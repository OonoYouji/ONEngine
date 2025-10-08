#include "GrassRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

GrassRenderingPipeline::GrassRenderingPipeline() = default;
GrassRenderingPipeline::~GrassRenderingPipeline() = default;



void GrassRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// Shader
		Shader shader;
		shader.Initialize(_shaderCompiler);

		/// mesh shader
		//shader.CompileShader(L"./Packages/Shader/Render/Grass/Grass.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"./Packages/Shader/Render/Grass/Grass.ms.hlsl", L"ms_6_5", Shader::Type::ms, L"MSMain");
		shader.CompileShader(L"./Packages/Shader/Render/Grass/Grass.ps.hlsl", L"ps_6_0", Shader::Type::ps, L"PSMain");

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		/// buffer
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); /// ROOT_PARAM_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); /// ROOT_PARAM_TIME

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

	{
		timeBuffer_.Create(_dxManager->GetDxDevice());
		timeBuffer_.SetMappedData({ 
			.windDirection = Vector3::kRight,
			.time = 0.0f, 
			.windStrength = 1.0f 
			});

		time_ = 0.0f;
	}

}

void GrassRenderingPipeline::Draw(ECSGroup* _ecs, const std::vector<class GameEntity*>& _entities, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// 描画コマンドをセット
	auto cmdList = _dxCommand->GetCommandList();

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	time_ += 1.0f / 60.0f;
	timeBuffer_.SetMappedData({
		.windDirection = Vector3::kFront,
		.time = time_,
		.windStrength = 1.0f
		});

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, ROOT_PARAM_VIEW_PROJECTION);
	timeBuffer_.BindForGraphicsCommandList(cmdList, ROOT_PARAM_TIME);

	cmdList->DispatchMesh(2, 1, 1);

}

