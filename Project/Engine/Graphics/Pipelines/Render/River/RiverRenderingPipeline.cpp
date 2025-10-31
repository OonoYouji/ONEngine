#include "RiverRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Asset/Collection/AssetCollection.h"

RiverRenderingPipeline::RiverRenderingPipeline(AssetCollection* _assetCollection) : pAssetCollection_(_assetCollection) {}
RiverRenderingPipeline::~RiverRenderingPipeline() = default;

void RiverRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/River/River.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/River/River.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);


		/// input element
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);


		/// buffer
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);

		pipeline_->AddDescriptorRange(0, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// texture
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		pipeline_->SetBlendDesc(BlendMode::None());
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void RiverRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// --------------------------------------------------------------------
	/// 早期リターンチェック
	/// --------------------------------------------------------------------

	ComponentArray<Terrain>* terrainArray = _ecs->GetComponentArray<Terrain>();
	if (!terrainArray || terrainArray->GetUsedComponents().empty()) {
		Console::LogError("RiverRenderingPipeline::Draw: Terrain component array is null");
		return;
	}

	Terrain* terrain = terrainArray->GetUsedComponents().front();
	if (!terrain) {
		return;
	}

	River* river = terrain->GetRiver();
	if (!river->GetIsCreatedBuffers()) {
		return;
	}


	/// --------------------------------------------------------------------
	/// bufferの設定
	/// --------------------------------------------------------------------

	auto cmdList = _dxCommand->GetCommandList();
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/// CBV_VIEW_PROJECTION
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, CBV_VIEW_PROJECTION);

	/// SRV_TEXTURE
	auto frontTex = pAssetCollection_->GetTextures().begin();
	cmdList->SetGraphicsRootDescriptorTable(SRV_TEXTURE, (*frontTex).GetSRVGPUHandle());

	/// CBV_MATERIAL
	river->SetMaterialData(terrain->GetOwner()->GetId(), static_cast<int32_t>(pAssetCollection_->GetTextureIndex("./Packages/Textures/Terrain/River.png")));
	river->GetMaterialBuffer().BindForGraphicsCommandList(cmdList, CBV_MATERIAL);


	/// VBVとIBVのリソースバリアーを変える
	river->CreateRenderingBarriers(_dxCommand);

	/// VBVとIBVの設定
	D3D12_VERTEX_BUFFER_VIEW vbv = river->CreateVBV();
	D3D12_INDEX_BUFFER_VIEW ibv = river->CreateIBV();
	cmdList->IASetVertexBuffers(0, 1, &vbv);
	cmdList->IASetIndexBuffer(&ibv);

	/// 描画
	cmdList->DrawIndexedInstanced(
		river->GetTotalIndices(),
		1, 0, 0, 0
	);

	/// 元の状態に戻す
	river->RestoreResourceBarriers(_dxCommand);

}

