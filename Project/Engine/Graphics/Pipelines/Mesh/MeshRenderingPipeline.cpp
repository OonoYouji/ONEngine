#include "MeshRenderingPipeline.h"

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Entity/Collection/EntityCollection.h"
#include "Engine/Component/Transform/Transform.h"
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"


MeshRenderingPipeline::MeshRenderingPipeline(GraphicsResourceCollection* _resourceCollection, EntityCollection* _entityCollection) 
	: resourceCollection_(_resourceCollection), entityCollection_(_entityCollection) {}

MeshRenderingPipeline::~MeshRenderingPipeline() {}

void MeshRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxDevice* _dxDevice) {

	{	/// pipeline create

		/// shader compile
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Assets/Shader/Mesh/Mesh.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Assets/Shader/Mesh/Mesh.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT);

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< transform
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); ///< view projection


		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable           = TRUE;
		blendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		pipeline_->SetBlendDesc(blendDesc);

		pipeline_->CreatePipeline(_dxDevice);

	}


	{	/// constant buffer create

		transformBuffer_ = std::make_unique<ConstantBuffer<Matrix4x4>>();
		transformBuffer_->Create(_dxDevice);

	}

}

//void MeshRenderingPipeline::PostDraw([[maybe_unused]] DxCommand* _dxCommand) {
//
//	/// 描画データが空なら描画する必要がないのでreturn
//	if (renderingDataList_.empty()) {
//		return;
//	}
//
//	pipeline_->SetPipelineStateForCommandList(_dxCommand);
//	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();
//
//	/// 共通のデータをセット
//	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	/// camera
//
//
//
//	/// 個々に必要なデータをセットし描画する
//	for (RenderingData* renderingData : renderingDataList_) {
//
//		const Mesh*  mesh      = resourceCollection_->GetMesh(renderingData->renderMeshId);
//		Transform*&& transform = renderingData->meshRenderer->GetOwner()->GetTransform();
//
//		/// vbv, ibvのセット
//		commandList->IASetVertexBuffers(0, 1, &mesh->GetVBV());
//		commandList->IASetIndexBuffer(&mesh->GetIBV());
//
//		/// buffer dataのセット
//		transformBuffer_->SetMappingData(transform->GetMatWorld());
//		commandList->SetGraphicsRootConstantBufferView(0, transformBuffer_->Get()->GetGPUVirtualAddress());
//
//		/// 描画
//		commandList->DrawIndexedInstanced(static_cast<UINT>(mesh->GetIndices().size()), 1, 0, 0, 0);
//	}
//
//}

void MeshRenderingPipeline::Draw(DxCommand* _dxCommand, EntityCollection* _entityCollection) {
	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();
	Camera* camera = _entityCollection->GetCameras()[0]; ///< TODO: 仮のカメラ取得

	/// pre draw
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	camera->GetViewProjectionBuffer()->BindForCommandList(commandList, 1);


	/// draw
	for (auto& entity : _entityCollection->GetEntities()) {
		MeshRenderer*&& meshRenderer = entity->GetComponent<MeshRenderer>();

		if (meshRenderer) {

			//const Mesh*&& mesh      = resourceCollection_->GetMesh(meshRenderer->GetMeshId());
			//Transform*&&  transform = entity->GetTransform();

		}
	}


	/// post draw


}

