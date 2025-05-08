#include "MeshShaderTest.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Entity/Camera/Camera.h"
#include "Game/Objects/Terrain/Terrain.h"


void MeshShaderTest::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	{

		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();

		pipeline_->SetShader(&shader);

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);


		/// buffer
		//pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< indices
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< vertices
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< indices

		//pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);       ///< vertices
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 1 - 1);       ///< vertices
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 2 - 1);       ///< indices

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); ///< buffer length
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_MESH, 1); ///< view projection


		pipeline_->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

	{
		vertexBuffer_.Create(1000 * 1000, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		indexBuffer_.Create(999 * 999 * 6 / 3, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		//meshletBuffer_.Create(1024, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

		std::vector<VSInput> vertices;
		vertices.push_back({ { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } });
		vertices.push_back({ { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } });
		vertices.push_back({ { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } });
		vertices.push_back({ { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } });
		vertices.push_back({ { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } });
		vertices.push_back({ { -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } });

		for (size_t i = 0; i < vertices.size(); i++) {
			vertexBuffer_.SetMappedData(i, vertices[i]);
		}

		std::vector<Index> indices;
		indices.push_back({ { 5, 1, 2 } });
		indices.push_back({ { 3, 4, 5 } });

		for (size_t i = 0; i < indices.size(); i++) {
			indexBuffer_.SetMappedData(i, indices[i]);
		}


		/*	std::vector<Meshlet> meshlets;
			meshlets.push_back({ 0, 0, 1, 3, { 0.0f, 0.0f, 0.0f }, 1.0f });
			meshlets.push_back({ 3, 1, 1, 3, { 0.0f, 0.0f, 0.0f }, 1.0f });

			for (size_t i = 0; i < meshlets.size(); i++) {
				meshletBuffer_.SetMappedData(i, meshlets[i]);
			}*/

		bufferLength_.Create(_dxManager->GetDxDevice());
		bufferLength_.SetMappedData(
			{ static_cast<uint32_t>(vertices.size()), static_cast<uint32_t>(indices.size()) });



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

	/// 頂点の設定
	for (size_t i = 0; i < terrain->GetVertices().size() - 1; ++i) {
		const Mesh::VertexData& v = terrain->GetVertices()[i];
		vertexBuffer_.SetMappedData(
			i,
			{
				v.position, v.uv, v.normal
			}
		);
	}


	/// インデックスの設定
	for (size_t i = 0; i < terrain->GetIndices().size(); i += 3) {
		indexBuffer_.SetMappedData(
			i / 3,
			{
				terrain->GetIndices()[i + 0],
				terrain->GetIndices()[i + 1],
				terrain->GetIndices()[i + 2]
			}
		);
	}

	bufferLength_.SetMappedData(
		{ static_cast<uint32_t>(terrain->GetVertices().size()),
		static_cast<uint32_t>(terrain->GetIndices().size() / 3) }
	);

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto command = _dxCommand->GetCommandList();

	_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(
		command, 3
	);

	//meshletBuffer_.BindToCommandList(0, command);
	vertexBuffer_.BindToCommandList(0, command);
	indexBuffer_.BindToCommandList(1, command);
	bufferLength_.BindForGraphicsCommandList(command, 2);

	command->DispatchMesh(1, 1, 1);
}
