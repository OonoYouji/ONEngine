#include "Line3DRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line3DRenderer.h"


Line3DRenderingPipeline::Line3DRenderingPipeline() {}
Line3DRenderingPipeline::~Line3DRenderingPipeline() {}

void Line3DRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	{	/// pipelineの作成

		/// shaderをコンパイル
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line3D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line3D.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_.reset(new GraphicsPipeline());
		pipeline_->SetShader(&shader);

		/// input element setting
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		/// rasterizer desc setting
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		/// buffer
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection

		/// topology type setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

		/// blend desc setting
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		pipeline_->SetBlendDesc(blendDesc);

		/// depth stencil desc setting
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		/// create pipeline
		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}


	/// verticesを最大数分メモリを確保
	vertices_.reserve(kMaxVertexNum_);

	/// vertex bufferの作成
	vertexBuffer_.CreateResource(_dxManager->GetDxDevice(), sizeof(VertexData) * kMaxVertexNum_);
	vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));

	vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * kMaxVertexNum_);
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexData));


}

void Line3DRenderingPipeline::Draw(class EntityComponentSystem* _ecs, const std::vector<IEntity*>& _entities, Camera* _camera, DxCommand* _dxCommand) {

	/// rendering dataの収集
	for (auto& entity : _entities) {
		Line3DRenderer* renderer = entity->GetComponent<Line3DRenderer>();

		if (renderer) {
			vertices_.insert(vertices_.end(), renderer->GetVertices().begin(), renderer->GetVertices().end());
		}
	}

	if (vertices_.empty()) { return; } ///< 描画するデータがない場合は、描画処理を行わない

	if (vertices_.size() > kMaxVertexNum_) { ///< 描画数が最大数を超える場合は超過分を削除
		vertices_.resize(kMaxVertexNum_);
	}

	/// データのコピー
	std::memcpy(mappingData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

	/// ここから描画処理
	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(commandList, 0);


	/// draw call
	commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);



	/// 描画データのクリア
	vertices_.clear();

}
