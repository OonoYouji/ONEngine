#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "Input/Input.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"

/// lib
#include "Debugger/Assertion.h"

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

	if(Input::TriggerKey(KeyCode::A)) {
		SetNextScene("TitleScene");
	}


}
