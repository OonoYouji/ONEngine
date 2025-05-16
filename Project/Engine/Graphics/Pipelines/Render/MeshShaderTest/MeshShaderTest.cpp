#define NOMINMAX
#include "MeshShaderTest.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Entity/Camera/Camera.h"


namespace {
	enum ROOT_PARAMETER {
		ROOT_PARAM_VERTEX,
		ROOT_PARAM_INDEX,
		ROOT_PARAM_MESHLET,
		ROOT_PARAM_UNIQUE_VERTEX_INDEX,
		ROOT_PARAM_VIEW_PROJECTION,
		ROOT_PARAM_MESH_INFO,
	};

	struct uint3 {
		uint32_t x, y, z;
	};

	uint3 UnpackPrimitive(uint32_t primitive) {
		// Unpacks a 10 bits per index triangle from a 32-bit uint.
		return uint3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
	}
}


void MeshShaderTest::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	{

		Shader shader;
		shader.Initialize(_shaderCompiler);

		//shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();

		pipeline_->SetShader(&shader);

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_WIREFRAME);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		/// buffer
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< vertices
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< indices
		pipeline_->AddDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< meshlet
		pipeline_->AddDescriptorRange(4, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< unique vertex indices

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 0);       ///< vertices
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 1);       ///< indices
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 2);       ///< meshlet
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 3);       ///< unique vertex indices

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_MESH, 1); ///< view projection
		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_MESH, 2); ///< mesh info


		pipeline_->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

	{
		vertexBuffer_.Create(1000 * 1000, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		primitiveBuffer_.Create(999 * 999 * 6, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		meshletBuffer_.Create(30000, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		//bufferLength_.Create(_dxManager->GetDxDevice());
		uniqueVertexIndices_.Create(999 * 999 * 6, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		//meshInfoBuffer_.Create(_dxManager->GetDxDevice());

	}

}

void MeshShaderTest::Draw(DxCommand* _dxCommand, [[maybe_unused]] EntityComponentSystem* _pEntityComponentSystem, [[maybe_unused]] Camera* _camera) {

	Terrain* terrain = nullptr;
	for (auto& entity : _pEntityComponentSystem->GetEntities()) {
		if (entity->GetName() == "Terrain") {
			terrain = static_cast<Terrain*>(entity.get());
			break;
		}
	}

	/// 地形が存在しないなら描画しない
	if (terrain == nullptr) {
		return;
	}

	/// 編集した頂点を更新する
	if (!terrain->GetEditVertices().empty()) {
		for (auto& editV : terrain->GetEditVertices()) {
			vertexBuffer_.SetMappedData(
				editV.first, { editV.second->position, editV.second->normal, editV.second->uv }
			);
		}
	}



	static bool isSetting = false;
	if (!isSetting) {
		isSetting = true;

		/// meshletの設定
		for (size_t i = 0; i < terrain->GetMeshlets().size(); i++) {
			meshletBuffer_.SetMappedData(
				i, terrain->GetMeshlets()[i].meshlet
			);
		}

		/// 頂点の設定
		for (size_t i = 0; i < terrain->GetVertices().size(); ++i) {
			const Mesh::VertexData& v = terrain->GetVertices()[i];
			vertexBuffer_.SetMappedData(
				i, { v.position, v.normal, v.uv }
			);
		}

		/// メッシュレットごとのローカルのインデックス
		size_t primitiveIndex = 0;
		for (size_t i = 0; i < terrain->GetMeshlets().size(); i++) {
			auto& meshlet = terrain->GetMeshlets()[i];
			for (size_t j = 0; j < meshlet.triangles.size(); j++) {
				auto& tri = meshlet.triangles[j];

				primitiveBuffer_.SetMappedData(
					primitiveIndex++, { PackPrimitive(tri.i0, tri.i1, tri.i2) }
				);
			}
		}

		/// インデックスの設定
		for (size_t i = 0; i < terrain->GetIndices().size(); i++) {
			uniqueVertexIndices_.SetMappedData(
				i, terrain->GetIndices()[i]
			);
		}
	}

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto command = _dxCommand->GetCommandList();

	vertexBuffer_.BindToCommandList(ROOT_PARAM_VERTEX, command); /// vertices
	primitiveBuffer_.BindToCommandList(ROOT_PARAM_INDEX, command);  /// indices
	meshletBuffer_.BindToCommandList(ROOT_PARAM_MESHLET, command); /// meshlet
	uniqueVertexIndices_.BindToCommandList(ROOT_PARAM_UNIQUE_VERTEX_INDEX, command);
	_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(
		command, ROOT_PARAM_VIEW_PROJECTION
	);


	uint32_t dispatchCount = 128;
	uint32_t totalMeshlets = static_cast<uint32_t>(terrain->GetMeshlets().size());
	for (uint32_t i = 0; i < totalMeshlets; i += dispatchCount) {
		uint32_t meshletOffset = i;
		uint32_t currentDispatchCount = std::min(dispatchCount, totalMeshlets - i);

		// オフセットを RootConstantで渡す
		command->SetGraphicsRoot32BitConstant(
			ROOT_PARAM_MESH_INFO, 
			meshletOffset, 0);

		// DispatchMesh 呼び出し
		command->DispatchMesh(currentDispatchCount, 1, 1);
	}

}

uint32_t MeshShaderTest::PackPrimitive(uint32_t i0, uint32_t i1, uint32_t i2) {
	uint32_t result = (i0 & 0x3FF) | ((i1 & 0x3FF) << 10) | ((i2 & 0x3FF) << 20);
	//uint3 unpacked = UnpackPrimitive(result);
	return result;
}
