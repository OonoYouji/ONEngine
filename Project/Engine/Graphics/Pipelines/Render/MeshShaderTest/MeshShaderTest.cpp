#include "MeshShaderTest.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

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
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);


		/// buffer
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< vertices
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< indices

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 0);       ///< vertices
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 1);       ///< indices

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_MESH, 0); ///< buffer length


		pipeline_->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

	{
		vertexBuffer_.Create(1024, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		indexBuffer_.Create(1024, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

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

		bufferLength_.Create(_dxManager->GetDxDevice());
		bufferLength_.SetMappingData({
			static_cast<uint32_t>(vertices.size()),
			static_cast<uint32_t>(indices.size()) 
		});
		//bufferLength_.Unmap(_dxManager->GetDxDevice());

	}

}

void MeshShaderTest::Draw(DxCommand* _dxCommand, [[maybe_unused]] EntityComponentSystem* _pEntityComponentSystem, [[maybe_unused]] Camera* _camera) {

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto command = _dxCommand->GetCommandList();

	vertexBuffer_.BindToCommandList(0, command);
	indexBuffer_.BindToCommandList(1, command);
	bufferLength_.BindForGraphicsCommandList(command, 2);
	command->DispatchMesh(1, 1, 1);


}
