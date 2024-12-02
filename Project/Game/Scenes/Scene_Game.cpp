#include "Scene_Game.h"

/// std
#include <memory>
#include <numbers>

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/PipelineState/ComputePipelineState.h"
#include "GraphicManager/TextureManager/TextureManager.h"

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


	const Texture& tex = TextureManager::GetInstance()->CreateUAVTexture(
		"test", Vec2(1,1), DXGI_FORMAT_R32G32B32A32_FLOAT
	);


	shader.reset(new ShaderBlob);
	shader->Compile(
		L"./Resources/Shaders/Compute/CreateWhite1x1TextureCS.hlsl",
		L"cs_6_6", ShaderBlob::CS
	);

	cpos.reset(new ComputePipelineState());
	cpos->SetShaderBlob(shader.get());
	
	cpos->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
	cpos->AddDescriptorTable(0);

	cpos->Create();
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	const Texture& texture = TextureManager::GetInstance()->GetTexture("test");

	ONE::DxCommon* dxCommon = ONEngine::GetDxCommon();
	ONE::DxCommand* dxCommand = dxCommon->GetDxCommand();
	ID3D12GraphicsCommandList* commandList = dxCommand->GetList();


	cpos->SetToCommnadList(commandList);
	dxCommon->GetSRVDescriptorHeap()->BindToCommandList(dxCommand->GetList());
	commandList->SetComputeRootDescriptorTable(0, texture.GetGPUHandle());
	commandList->Dispatch(1, 1, 1);

	dxCommand->Close();
	dxCommand->Execution();
	dxCommand->Reset();
}
