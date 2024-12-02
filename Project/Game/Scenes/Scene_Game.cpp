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

#include "GraphicManager/PostEffect/PostEffectPipeline/PostEffectPipelineManager.h"
#include "GraphicManager/PostEffect/Grayscale/Grayscale.h"



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


}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
}
