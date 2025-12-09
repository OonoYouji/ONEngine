#include "GizmoRenderingPipeline.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Tools/Gizmo.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

using namespace GizmoPrimitive;

GizmoRenderingPipeline::GizmoRenderingPipeline() {}

void GizmoRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	Gizmo::Initialize(static_cast<size_t>(std::pow(2, 20))); /// gizmoの初期化

	{	/// wire frame pipeline
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line3D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line3D.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipelines_[Wire] = std::make_unique<GraphicsPipeline>();
		auto pipeline = pipelines_[Wire].get();

		pipeline->SetShader(&shader);

		/// input element setting
		pipeline->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline->SetBlendDesc(BlendMode::None());
		pipeline->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline->SetDepthStencilDesc(depthStencilDesc);

		/// create pipeline
		pipeline->CreatePipeline(_dxm->GetDxDevice());
	}


	{
		/// verticesを最大数分メモリを確保
		maxVertexNum_ = static_cast<size_t>(std::pow(2, 18));
		vertices_.reserve(maxVertexNum_);

		/// vertex bufferの作成
		vertexBuffer_.CreateResource(_dxm->GetDxDevice(), sizeof(VertexData) * maxVertexNum_);
		vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));

		vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
		vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * maxVertexNum_);
		vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexData));

	}
}

void GizmoRenderingPipeline::Draw(class ECSGroup* /*_ecsGroup*/, [[maybe_unused]] CameraComponent* _camera, [[maybe_unused]] DxCommand* _dxCommand) {
#ifdef DEBUG_MODE

	if (_camera->GetOwner()->GetECSGroup()->GetGroupName() != "Debug") {
		return;
	}

	/// ---------------------------------------------------
	/// wire描画を行う
	/// ---------------------------------------------------

	const std::vector<Gizmo::SphereData>& wireSphereData = Gizmo::GetWireSphereData();
	const std::vector<Gizmo::CubeData>& wireCubeData = Gizmo::GetWireCubeData();
	const std::vector<Gizmo::LineData>& lineData = Gizmo::GetLineData();

	///!< 描画対象がなければ 早期リターン
	if (wireSphereData.empty() && wireCubeData.empty() && lineData.empty()) {
		return;
	}

	std::vector<VertexData> vertices;
	/// sphereのデータを頂点データに積む
	for (auto& data : wireSphereData) {
		vertices = GetSphereVertices(data.position, data.radius, data.color, 12);
		vertices_.insert(vertices_.end(), vertices.begin(), vertices.end());
	}

	/// cubeのデータを頂点データに積む
	for (auto& data : wireCubeData) {
		vertices = GetCubeVertices(data.position, data.size, data.color);
		vertices_.insert(vertices_.end(), vertices.begin(), vertices.end());
	}

	/// lineのデータを頂点データに積む
	for (auto& data : lineData) {
		VertexData v0, v1;
		v0.position = Vector4(data.startPosition, 1.0f);
		v1.position = Vector4(data.endPosition, 1.0f);
		v0.color = data.color;
		v1.color = data.color;
		vertices_.push_back(v0);
		vertices_.push_back(v1);
	}

	/// 超過した分を削除
	if (vertices_.size() > maxVertexNum_) {
		vertices_.resize(maxVertexNum_);
	}


	std::memcpy(mappingData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

	/// 描画命令を行う
	auto commandList = _dxCommand->GetCommandList();
	auto wirePipeline = pipelines_[Wire].get();
	wirePipeline->SetPipelineStateForCommandList(_dxCommand);

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(commandList, 0);

	/// draw call
	commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);

	/// 描画データのクリア
	Gizmo::Reset();
	vertices_.clear();
#endif _DEBUG
}

