#include "GizmoRenderingPipeline.h"

using namespace ONEngine;

/// std
#include <numbers>

/// engine
#include "Engine/Core/Utility/Tools/Log.h"
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
		shader.CompileShader(L"./Packages/Shader/Render/Line/Gizmo3D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Gizmo3D.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipelines_[Wire] = std::make_unique<GraphicsPipeline>();
		auto pipeline = pipelines_[Wire].get();

		pipeline->SetShader(&shader);

		/// input element setting
		pipeline->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline->AddInputElement("OTHER_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline->AddInputElement("THICKNESS", 0, DXGI_FORMAT_R32_FLOAT);
		pipeline->AddInputElement("EXPANSION_DIR", 0, DXGI_FORMAT_R32G32_FLOAT);

		pipeline->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline->SetBlendDesc(BlendMode::None());
		pipeline->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection

		pipeline->SetDepthStencilDesc(DefaultDepthStencilDesc());

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
		/* 
		// 特定のカメラグループのみに限定すると見えない可能性があるため一旦コメントアウト
		if (_camera->GetOwner()->GetECSGroup()->GetGroupName() != "Debug") {
			return;
		}
		*/

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

	// 今回の描画内容をログ出力 (最初の10回)
	static int drawLogCount = 0;
	if (drawLogCount < 10) {
		ONEngine::Console::Log("[Gizmo Pipeline] Drawing " + std::to_string(lineData.size()) + " lines.", ONEngine::LogCategory::Engine);
		drawLogCount++;
	}

	std::vector<VertexData> vertices;
	/// sphereのデータを頂点データに積む
	for (auto& data : wireSphereData) {
		vertices = GetSphereVertices(data.position, data.radius, data.color, 1.0f, 12); // 太さ1.0固定（必要なら拡張）
		vertices_.insert(vertices_.end(), vertices.begin(), vertices.end());
	}

	/// cubeのデータを頂点データに積む
	for (auto& data : wireCubeData) {
		vertices = GetCubeVertices(data.position, data.size, data.rotate, data.color, 1.0f); // 太さ1.0固定
		vertices_.insert(vertices_.end(), vertices.begin(), vertices.end());
	}

	/// lineのデータを頂点データに積む
	for (auto& data : lineData) {
		Vector4 p0 = Math::ConvertToVector4(data.startPosition, 1.0f);
		Vector4 p1 = Math::ConvertToVector4(data.endPosition, 1.0f);

		VertexData v[4];
		for (int i = 0; i < 4; ++i) {
			v[i].position = p0;      // 常に始点
			v[i].otherPosition = p1; // 常に終点
			v[i].color = data.color;
			v[i].thickness = data.thickness;
		}

		v[0].expansionDir = Vector2(-1.0f, 0.0f); // 始点・左
		v[1].expansionDir = Vector2(1.0f, 0.0f);  // 始点・右
		v[2].expansionDir = Vector2(-1.0f, 1.0f); // 終点・左
		v[3].expansionDir = Vector2(1.0f, 1.0f);  // 終点・右

		// Tri 1: (0, 2, 1)
		vertices_.push_back(v[0]); vertices_.push_back(v[2]); vertices_.push_back(v[1]);
		// Tri 2: (1, 2, 3)
		vertices_.push_back(v[1]); vertices_.push_back(v[2]); vertices_.push_back(v[3]);
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
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(commandList, 0);

	/// draw call
	commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);

	/// 描画データのクリア
	Gizmo::Reset();
	vertices_.clear();
#endif _DEBUG
}

