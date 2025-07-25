#include "EffectRenderingPipeline.h"

/// std
#include <unordered_map>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Effect/Effect.h"


EffectRenderingPipeline::EffectRenderingPipeline(GraphicsResourceCollection* _resourceCollection)
	: pResourceCollection_(_resourceCollection) {}
EffectRenderingPipeline::~EffectRenderingPipeline() {}

void EffectRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

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
			pipeline->AddDescriptorRange(2, 32, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
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

			pipeline->CreatePipeline(_dxManager->GetDxDevice());

		}
	}


	{	/// buffer create

		transformBuffer_ = std::make_unique<StructuredBuffer<Matrix4x4>>();
		transformBuffer_->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

		materialBuffer = std::make_unique<StructuredBuffer<Vector4>>();
		materialBuffer->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

		textureIdBuffer_ = std::make_unique<StructuredBuffer<uint32_t>>();
		textureIdBuffer_->Create(static_cast<uint32_t>(kMaxRenderingMeshCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

	}
}


void EffectRenderingPipeline::Draw(const std::vector<IEntity*>& _entities, Camera* _camera, DxCommand* _dxCommand) {

	std::unordered_map<size_t, std::unordered_map<std::string, std::list<Effect*>>> blendMeshEffectMap;

	/// mesh と transform の対応付け
	for (auto& entity : _entities) {
		Effect*&& effect = entity->GetComponent<Effect>();
		if (effect) {
			blendMeshEffectMap[static_cast<size_t>(effect->GetBlendMode())][effect->GetMeshPath()].push_back(effect);
		}
	}

	///< 描画対象がなければ 早期リターン
	if (blendMeshEffectMap.empty()) {
		return;
	}


	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();

	/// settings
	//pipeline_->SetPipelineStateForCommandList(_dxCommand);

	transformIndex_ = 0;
	instanceIndex_ = 0;

	for (auto& [blendMode, meshPerComp] : blendMeshEffectMap) {
		pipelines_[blendMode]->SetPipelineStateForCommandList(_dxCommand);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(commandList, 0);

		/// buffer dataのセット、先頭の texture gpu handle をセットする
		auto& textures = pResourceCollection_->GetTextures();
		commandList->SetGraphicsRootDescriptorTable(3, (*textures.begin())->GetSRVGPUHandle());


		for (auto& [meshPath, effects] : meshPerComp) {

			/// modelの取得、なければ次へ
			const Model*&& model = pResourceCollection_->GetModel(meshPath);
			if (!model) {
				continue;
			}

			/// transform, material を mapping
			for (auto& effect : effects) {
				if (effect->GetElements().empty()) {
					continue;
				}

				for (auto& element : effect->GetElements()) {

					/// materialのセット
					materialBuffer->SetMappedData(
						transformIndex_,
						element.color
					);

					/// texture id のセット
					size_t textureIndex = pResourceCollection_->GetTextureIndex(effect->GetTexturePath());
					textureIdBuffer_->SetMappedData(
						transformIndex_,
						textures[textureIndex]->GetSRVDescriptorIndex()
					);

					/// transform のセット
					transformBuffer_->SetMappedData(
						transformIndex_,
						element.transform.GetMatWorld()
					);

					++transformIndex_;
				}

				/// 上でセットしたデータをバインド
				materialBuffer->BindToCommandList(1, commandList);
				textureIdBuffer_->BindToCommandList(2, commandList);
				transformBuffer_->BindToCommandList(4, commandList);

				/// 現在のinstance idをセット
				commandList->SetGraphicsRoot32BitConstant(5, instanceIndex_, 0);

				/// mesh の描画
				for (auto& mesh : model->GetMeshes()) {
					/// vbv, ibvのセット
					commandList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
					commandList->IASetIndexBuffer(&mesh->GetIBV());

					/// 描画
					commandList->DrawIndexedInstanced(
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


