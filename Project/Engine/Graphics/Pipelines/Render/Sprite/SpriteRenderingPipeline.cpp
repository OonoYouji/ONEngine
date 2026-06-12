#include "SpriteRenderingPipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/GPUTimeStamp/GPUTimeStamp.h"




SpriteRenderingPipeline::SpriteRenderingPipeline(Asset::AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {
}
SpriteRenderingPipeline::~SpriteRenderingPipeline() {}


void SpriteRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	pDxManager_ = _dxm;
	{	/// pipeline 

		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"Packages/Shader/Render/Sprite/Sprite.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Packages/Shader/Render/Sprite/Sprite.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->SetRTVNum(static_cast<uint32_t>(RTVIndex::Count));
		pipeline_->SetRTVFormat(static_cast<DXGI_FORMAT>(RTVFormat::Color), static_cast<int>(RTVIndex::Color));
		pipeline_->SetRTVFormat(static_cast<DXGI_FORMAT>(RTVFormat::WorldPosition), static_cast<int>(RTVIndex::WorldPosition));
		pipeline_->SetRTVFormat(static_cast<DXGI_FORMAT>(RTVFormat::Normal), static_cast<int>(RTVIndex::Normal));
		pipeline_->SetRTVFormat(static_cast<DXGI_FORMAT>(RTVFormat::Flags), static_cast<int>(RTVIndex::Flags));


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);                  ///< ROOT_PARAM_VIEW_PROJECTION : 0

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< ROOT_PARAM_MATERIAL
		pipeline_->AddDescriptorRange(1, Asset::MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< ROOT_PARAM_TEXTURES
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< ROOT_PARAM_TRANSFORM
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);       ///< ROOT_PARAM_MATERIAL : 1
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);       ///< ROOT_PARAM_TEXTURES   : 2
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 2);      ///< ROOT_PARAM_TRANSFORM : 3

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);         ///< texture sampler
		pipeline_->SetBlendDesc(BlendMode::Normal());

		pipeline_->SetDepthStencilDesc(DepthNone()); // 2D UIなので深度テストを無効化

		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}


	{	/// buffer

		/// ----- Spriteに使用する頂点とインデックスの情報を作る ----- ///
		/// vertex data
		vertices_ = {
			{ Vector4(-0.5f, 0.5f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },
			{ Vector4(0.5f, 0.5f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },
			{ Vector4(-0.5f, -0.5f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
			{ Vector4(0.5f, -0.5f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
		};

		indices_ = {
			0, 1, 2, ///< 1面
			2, 1, 3, ///< 2面
		};

		const size_t kVertexDataSize = sizeof(VertexData);

		/// vertex buffer
		vertexBuffer_.CreateResource(_dxm->GetDxDevice(), kVertexDataSize * vertices_.size());

		vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
		vbv_.SizeInBytes = static_cast<UINT>(kVertexDataSize * vertices_.size());
		vbv_.StrideInBytes = static_cast<UINT>(kVertexDataSize);

		/// index buffer
		indexBuffer_.CreateResource(_dxm->GetDxDevice(), sizeof(uint32_t) * indices_.size());

		ibv_.BufferLocation = indexBuffer_.Get()->GetGPUVirtualAddress();
		ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
		ibv_.Format = DXGI_FORMAT_R32_UINT;


		/// mapping
		VertexData* vertexData = nullptr;
		vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
		memcpy(vertexData, vertices_.data(), kVertexDataSize * vertices_.size());

		uint32_t* indexData = nullptr;
		indexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
		memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());

	}


	{	/// structured buffer
		transformsBuffer_.Create(static_cast<uint32_t>(kMaxRenderingSpriteCount_), _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
		materialsBuffer.Create(static_cast<uint32_t>(kMaxRenderingSpriteCount_), _dxm->GetDxDevice(), _dxm->GetDxSRVHeap());
	}


}

void SpriteRenderingPipeline::Draw(class ECSGroup* _ecsGroup, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// SpriteRendererの配列の取得&存在チェック
	ComponentArray<SpriteRenderer>* spriteRendererArray = _ecsGroup->GetComponentArray<SpriteRenderer>();
	if (!spriteRendererArray || spriteRendererArray->GetUsedComponents().empty()) {
		return;
	}

	GPUTimeStamp::GetInstance().BeginTimeStamp(GPUTimeStampID::SpriteRendering);

	/// 描画データの収集とソート用構造体
	struct RenderingData {
		SpriteRenderer* renderer;
		float z;
		Matrix4x4 matWorld;
	};
	std::vector<RenderingData> renderingDataList;
	renderingDataList.reserve(spriteRendererArray->GetUsedComponents().size());

	/// 描画対象を収集
	for (auto& sr : spriteRendererArray->GetUsedComponents()) {
		if (!CheckComponentEnable(sr)) {
			continue;
		}

		if (GameEntity* owner = sr->GetOwner()) {
			/// setup
			sr->RenderingSetup(pAssetCollection_);
			
			renderingDataList.push_back({
				sr,
				owner->GetTransform()->position.z, // Z値を保存
				owner->GetTransform()->GetMatWorld()
			});
		}
	}

	/// Z値でソート (大きい順: 奥から手前へ)
	std::sort(renderingDataList.begin(), renderingDataList.end(), [](const RenderingData& a, const RenderingData& b) {
		return a.z > b.z;
	});

	/// bufferにデータをセット
	size_t transformIndex = 0;
	for (const auto& data : renderingDataList) {
		/// Material, Transformのセット
		materialsBuffer.SetMappedData(transformIndex, data.renderer->GetGpuMaterial());
		transformsBuffer_.SetMappedData(transformIndex, data.matWorld);
		++transformIndex;
	}

	// 検証用ログ (最初の10回)
	static int spriteLogCount = 0;
	if (spriteLogCount < 10) {
		Console::Log("[SpritePipeline] Drawing " + std::to_string(transformIndex) + " sorted sprites.", LogCategory::Engine);
		spriteLogCount++;
	}

	/// 初期値のままなら描画対象なしなので描画しない
	if (transformIndex == 0) {
		return;
	}


	auto cmdList = _dxCommand->GetCommandList();

	/// settings
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	/// vbv, ivb setting
	cmdList->IASetVertexBuffers(0, 1, &vbv_);
	cmdList->IASetIndexBuffer(&ibv_);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/// ROOT_PARAM_VIEW_PROJECTION : 0
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, ROOT_PARAM_VIEW_PROJECTION);

	/// テクスチャ記述子ヒープの開始ハンドルをセットする
	/// (Shader側で textures[material.baseTextureId] としてアクセスするため)
	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_TEXTURES, pDxManager_->GetDxSRVHeap()->GetSRVStartGPUHandle());

	materialsBuffer.SRVBindForGraphicsCommandList(cmdList, ROOT_PARAM_MATERIAL);
	transformsBuffer_.SRVBindForGraphicsCommandList(cmdList, ROOT_PARAM_TRANSFORM);

	/// 描画
	cmdList->DrawIndexedInstanced(
		static_cast<UINT>(indices_.size()),
		static_cast<UINT>(transformIndex),
		0, 0, 0
	);

	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::SpriteRendering);
}

