#include "GizmoRenderingPipeline.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Tools/Gizmo.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"

GizmoRenderingPipeline::GizmoRenderingPipeline() {}

void GizmoRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	Gizmo::Initialize(1000); /// gizmoの初期化

	{	/// wire frame pipeline
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Assets/Shader/Line/Line3D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Assets/Shader/Line/Line3D.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipelines_[Wire] = std::make_unique<GraphicsPipeline>();
		auto pipeline = pipelines_[Wire].get();

		pipeline->SetShader(&shader);

		/// input element setting
		pipeline->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline->SetBlendDesc(BlendMode::Normal());
		pipeline->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline->SetDepthStencilDesc(depthStencilDesc);

		pipeline->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);

		/// create pipeline
		pipeline->CreatePipeline(_dxManager->GetDxDevice());
	}


	//{	/// solid pipeline create

	//	Shader shader;
	//	shader.Initialize(_shaderCompiler);



	//}


	{
		/// verticesを最大数分メモリを確保
		maxVertexNum_ = static_cast<size_t>(std::pow(2, 16));
		vertices_.reserve(maxVertexNum_);

		/// vertex bufferの作成
		vertexBuffer_.CreateResource(_dxManager->GetDxDevice(), sizeof(VertexData) * maxVertexNum_);
		vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));

		vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
		vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * maxVertexNum_);
		vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexData));

	}
}

void GizmoRenderingPipeline::Draw(DxCommand* _dxCommand, [[maybe_unused]] EntityComponentSystem* _entityComponentSystem, Camera* _camera) {

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
	_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(commandList, 0);

	/// draw call
	commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);

	/// 描画データのクリア
	Gizmo::Reset();
	vertices_.clear();
}

std::vector<GizmoRenderingPipeline::VertexData> GizmoRenderingPipeline::GetSphereVertices(const Vector3& _center, float _radius, const Vector4& _color, size_t _segment) {
	const float deltaAngle = 2.0f * std::numbers::pi_v<float> / _segment;
	std::vector<VertexData> outVertices;

	auto addCircle = [&](const Vector3& _axis1, const Vector3& _axis2) {
		for (int i = 0; i < _segment; ++i) {
			float angle0 = i * deltaAngle;
			float angle1 = (i + 1) * deltaAngle;

			Vector3 dir0 = Vector3::Normalize(_axis1 * std::cos(angle0) + _axis2 * std::sin(angle0));
			Vector3 dir1 = Vector3::Normalize(_axis1 * std::cos(angle1) + _axis2 * std::sin(angle1));

			VertexData v0;
			v0.position = Vector4(_center + dir0 * _radius, 1.0f);
			v0.color = _color;

			VertexData v1;
			v1.position = Vector4(_center + dir1 * _radius, 1.0f);
			v1.color = _color;

			outVertices.push_back(v0);
			outVertices.push_back(v1);
		}
		};

	// XY平面
	addCircle(Vector3::kRight, Vector3::kUp);
	// YZ平面
	addCircle(Vector3::kUp, Vector3::kFront);
	// ZX平面
	addCircle(Vector3::kFront, Vector3::kRight);

	return outVertices;
}

std::vector<GizmoRenderingPipeline::VertexData> GizmoRenderingPipeline::GetCubeVertices(const Vector3& _center, const Vector3& _size, const Vector4& _color) {
	Vector3 halfSize = _size * 0.5f;
	std::vector<VertexData> outVertices;

	// 立方体の8頂点
	Vector3 vertices[8] = {
		_center + Vector3(-halfSize.x, -halfSize.y, -halfSize.z), // 0
		_center + Vector3(halfSize.x, -halfSize.y, -halfSize.z), // 1
		_center + Vector3(halfSize.x, halfSize.y, -halfSize.z), // 2
		_center + Vector3(-halfSize.x, halfSize.y, -halfSize.z), // 3
		_center + Vector3(-halfSize.x, -halfSize.y, halfSize.z), // 4
		_center + Vector3(halfSize.x, -halfSize.y, halfSize.z), // 5
		_center + Vector3(halfSize.x, halfSize.y, halfSize.z), // 6
		_center + Vector3(-halfSize.x, halfSize.y, halfSize.z)  // 7
	};

	// 線を引く頂点のペアリスト
	int indices[] = {
		0, 1, 1, 2, 2, 3, 3, 0, // 底面
		4, 5, 5, 6, 6, 7, 7, 4, // 上面
		0, 4, 1, 5, 2, 6, 3, 7  // 側面
	};

	VertexData v0, v1;
	for (int i = 0; i < sizeof(indices) / sizeof(int); i += 2) {
		v0.position = Vector4(vertices[indices[i + 0]], 1.0f);
		v0.color = _color;

		v1.position = Vector4(vertices[indices[i + 1]], 1.0f);
		v1.color = _color;

		outVertices.push_back(v0);
		outVertices.push_back(v1);
	}

	return outVertices;
}

