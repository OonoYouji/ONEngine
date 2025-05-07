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

		pipeline_->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}
}

void MeshShaderTest::Draw(DxCommand* _dxCommand, [[maybe_unused]] EntityComponentSystem* _pEntityComponentSystem, [[maybe_unused]] Camera* _camera) {

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto command = _dxCommand->GetCommandList();

	command->DispatchMesh(1, 1, 1);


}
