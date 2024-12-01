#include "Scene_Game.h"

/// std
#include <memory>
#include <numbers>

/// engine
#include "GraphicManager/PipelineState/ComputePipelineState.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	//(new DemoObject)->Initialize();

	mainCamera_->SetPosition({ 0, 5.5f, -17.0f });
	mainCamera_->SetRotate({ 0.25f, 0.0f, 0.0f });

	std::unique_ptr<ComputePipelineState> cpos;
	std::unique_ptr<ShaderBlob> shader;

	shader.reset(new ShaderBlob);
	shader->Compile(
		L"./Resources/Shaders/Compute/SampleCS.hlsl",
		L"cs_6_6", ShaderBlob::CS
	);

	cpos.reset(new ComputePipelineState());
	cpos->SetShaderBlob(shader.get());
	cpos->AddCBV(0);
	
	cpos->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	cpos->AddDescriptorTable(0);

	cpos->AddStaticSampler(0);

	cpos->Create();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {


}
