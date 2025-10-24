#include "EffectRenderingPipeline.h"

/// std
#include <unordered_map>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Effect/Effect.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


EffectRenderingPipeline::EffectRenderingPipeline(AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {
}
EffectRenderingPipeline::~EffectRenderingPipeline() {}

void EffectRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{
		/// shader compile
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/Render/Effect/Effect.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Packages/Shader/Render/Effect/Effect.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		std::array<std::function<D3D12_BLEND_DESC()>, 5> blendModeFuncs{
			BlendMode::Normal,
			BlendMode::Add,
			BlendMode::Subtract,
			BlendMode::Multiply,
			BlendMode::Screen,
		};


		/// BlendModeごとにパイプラインを生成
		for (size_t i = 0; i < blendModeFuncs.size(); i++) {
			auto& pipeline = pipelines_[i];

			pipeline = std::make_unique<GraphicsPipeline>();
			pipeline->SetShader(&shader);

			pipeline->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
			pipeline->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
			pipeline->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

			pipeline->SetFillMode(D3D12_FILL_MODE_SOLID);
			pipeline->SetCullMode(D3D12_CULL_MODE_NONE);

			pipeline->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

			pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection: 0

			pipeline->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< material
			pipeline->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< textureId
			pipeline->AddDescriptorRange(2, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
			pipeline->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< transform
			pipeline->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);       ///< material  : 1
			pipeline->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);       ///< textureId : 2
			pipeline->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);       ///< texture   : 3
			pipeline->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 3);      ///< transform : 4

			pipeline->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);         ///< texture sampler

			pipeline->Add32BitConstant(D3D12_SHADER_VISIBILITY_VERTEX, 1);        ///< instance id: 5

			pipeline->SetBlendDesc(blendModeFuncs[i]());

			D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			pipeline->SetDepthStencilDesc(depthStencilDesc);

			pipeline->CreatePipeline(_dxm->GetDxDevice());

		}
	}


	{	/// buffer create

		transformBuffer_.Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
		materialBuffer_.Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
		textureIdBuffer_.Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());

	}
}


void EffectRenderingPipeline::Draw(class ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<Effect>* effectArray = _ecs->GetComponentArray<Effect>();
	if (!effectArray || effectArray->GetUsedComponents().empty()) {
		Console::LogError("EffectRenderingPipeline::Draw: Effect component array is null");
		return;
	}


	auto cmdList = _dxCommand->GetCommandList();

	transformIndex_ = 0;
	instanceIndex_ = 0;


	using MeshEffectMap = std::unordered_map<std::string, std::list<Effect*>>;
	std::unordered_map<size_t, MeshEffectMap> meshEffectsByBlendMode;

	for (const auto& effect : effectArray->GetUsedComponents()) {
		/// ----- EffectをBlendModeごとに仕分け、さらにMeshごとにListを作る ----- ///
		if (!effect || effect->enable) {
			continue;
		}

		size_t blendMode = static_cast<size_t>(effect->GetBlendMode());
		meshEffectsByBlendMode[blendMode][effect->GetMeshPath()].push_back(effect);
	}


	for (auto& [blendMode, meshPerComp] : meshEffectsByBlendMode) {

		/// 対応するBlendModeのパイプラインをセット
		pipelines_[blendMode]->SetPipelineStateForCommandList(_dxCommand);

		/// 形状、ビュー射影行列のセット
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, CBV_VIEW_PROJECTION);

		/// テクスチャの先頭を設定
		auto& textures = pAssetCollection_->GetTextures();
		cmdList->SetGraphicsRootDescriptorTable(SRV_TEXTURES, (*textures.begin()).GetSRVGPUHandle());


		for (auto& [meshPath, effects] : meshPerComp) {

			/// modelの取得、なければ次へ
			const Model*&& model = pAssetCollection_->GetModel(meshPath);
			if (!model) {
				continue;
			}

			/// transform, material を mapping
			for (auto& effect : effects) {
				if (effect->GetElements().empty()) {
					continue;
				}


				/// element ごとにデータセット
				for (auto& element : effect->GetElements()) {

					/// materialのセット
					materialBuffer_.SetMappedData(
						transformIndex_,
						element.color
					);

					/// texture id のセット
					size_t textureIndex = pAssetCollection_->GetTextureIndex(effect->GetTexturePath());
					textureIdBuffer_.SetMappedData(
						transformIndex_,
						textures[textureIndex].GetSRVDescriptorIndex()
					);

					/// transform のセット
					transformBuffer_.SetMappedData(
						transformIndex_,
						element.transform.GetMatWorld()
					);

					++transformIndex_;
				}

				/// 上でセットしたデータをバインド
				materialBuffer_.SRVBindForGraphicsCommandList(cmdList, SRV_MATERIALS);
				textureIdBuffer_.SRVBindForGraphicsCommandList(cmdList, SRV_TEXTURE_IDS);
				transformBuffer_.SRVBindForGraphicsCommandList(cmdList, SRV_TRANSFORMS);

				/// 現在のinstance idをセット
				cmdList->SetGraphicsRoot32BitConstant(C32BIT_CONSTANT, instanceIndex_, 0);

				/// mesh の描画
				for (auto& mesh : model->GetMeshes()) {
					/// vbv, ibvのセット
					cmdList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
					cmdList->IASetIndexBuffer(&mesh->GetIBV());

					/// 描画
					cmdList->DrawIndexedInstanced(
						static_cast<UINT>(mesh->GetIndices().size()),
						static_cast<UINT>(effect->GetElements().size()),
						0, 0, 0
					);
				}

				instanceIndex_ += static_cast<UINT>(effect->GetElements().size());
			}
		}
	}


}


