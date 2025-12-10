#include "Line3DRenderingPipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line3DRenderer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


Line3DRenderingPipeline::Line3DRenderingPipeline() {}
Line3DRenderingPipeline::~Line3DRenderingPipeline() {}

void Line3DRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	{	/// pipelineの作成

		/// shaderをコンパイル
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line3D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line3D.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_.reset(new GraphicsPipeline());
		pipeline_->SetShader(&shader);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		/// create pipeline
		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}


	/// verticesを最大数分メモリを確保
	vertices_.reserve(kMaxVertexNum_);

	/// vertex bufferの作成
	vertexBuffer_.CreateResource(_dxm->GetDxDevice(), sizeof(VertexData) * kMaxVertexNum_);
	vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));

	vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * kMaxVertexNum_);
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexData));


}

void Line3DRenderingPipeline::Draw(class ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<Line3DRenderer>* line3DRendererArray = _ecs->GetComponentArray<Line3DRenderer>();
	if (!line3DRendererArray || line3DRendererArray->GetUsedComponents().empty()) {
		return;
	}

	/// rendering dataの収集
	for (auto& renderer : line3DRendererArray->GetUsedComponents()) {
		if (renderer) {
			vertices_.insert(vertices_.end(), renderer->GetVertices().begin(), renderer->GetVertices().end());
		}
	}

	/// 描画するデータがない場合は、描画処理を行わない
	if (vertices_.empty()) { return; }

	/// 描画数が最大数を超える場合は超過分を削除
	if (vertices_.size() > kMaxVertexNum_) {
		vertices_.resize(kMaxVertexNum_);
	}

	/// データのコピー
	std::memcpy(mappingData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

	/// ここから描画処理
	auto cmdList = _dxCommand->GetCommandList();

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	cmdList->IASetVertexBuffers(0, 1, &vbv_);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, 0);

	/// 描画
	cmdList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);

	/// 描画データのクリア
	vertices_.clear();
}
