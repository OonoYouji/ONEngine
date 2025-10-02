#include "RiverRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

RiverRenderingPipeline::RiverRenderingPipeline() = default;
RiverRenderingPipeline::~RiverRenderingPipeline() = default;

void RiverRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

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



		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}

}

void RiverRenderingPipeline::Draw(ECSGroup* _ecs, const std::vector<GameEntity*>& _entities, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<Terrain>* terrainArray = _ecs->GetComponentArray<Terrain>();
	if(!terrainArray) {
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


	auto cmdList = _dxCommand->GetCommandList();
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, 0);
	/// vbvとibvのリソースバリアーを変える
	DxResource& vertexBuffer = river->GetRwVerticesRef().GetResource();
	vertexBuffer.CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		_dxCommand
	);

	/// vbv setting
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = vertexBuffer.Get()->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(RiverVertex);
	vbv.SizeInBytes = sizeof(RiverVertex) * river->GetTotalVertices();
	cmdList->IASetVertexBuffers(0, 1, &vbv);


	DxResource& indexBuffer = river->GetRwIndicesRef().GetResource();
	indexBuffer.CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		_dxCommand
	);

	/// ibv setting
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = indexBuffer.Get()->GetGPUVirtualAddress();
	ibv.SizeInBytes = static_cast<UINT>(sizeof(uint32_t)) * river->GetTotalIndices();
	ibv.Format = DXGI_FORMAT_R32_UINT;
	cmdList->IASetIndexBuffer(&ibv);


	cmdList->DrawIndexedInstanced(
		river->GetTotalIndices(),
		1, 0, 0, 0
	);

	/// 元の状態に戻す
	vertexBuffer.CreateBarrier(
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		_dxCommand
	);

	indexBuffer.CreateBarrier(
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		_dxCommand
	);
}

