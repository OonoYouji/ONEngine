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

	(new DemoObject)->Initialize();

	mainCamera_->SetPosition({ 0, 5.5f, -17.0f });
	mainCamera_->SetRotate({ 0.25f, 0.0f, 0.0f });




	grayscale_.reset(new Grayscale());
	grayscale_->Initialize();

	TextureManager::GetInstance()->Load("grayscaleTestTexture", "grayscaleTestTexture.png");
	input_ = TextureManager::GetInstance()->GetTexture("grayscaleTestTexture");
	output_ = TextureManager::GetInstance()->CreateUAVTexture("testOutputTex", { 1280, 720 }, DXGI_FORMAT_R32G32B32A32_FLOAT);

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	ID3D12GraphicsCommandList* pCommandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	ONEngine::GetDxCommon()->GetSRVDescriptorHeap()->BindToCommandList(pCommandList);
	

	grayscale_->Execution(input_, output_);


	auto command = ONEngine::GetDxCommon()->GetDxCommand();
	command->Close();
	command->Execution();
	command->Reset();
}
