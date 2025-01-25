#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "Input/Input.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"
#include "Objects/Grid/Grid.h"

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	(new Grid)->Initialize();

	(new RedBall)->Initialize();
	(new WhiteBall)->Initialize();


	mainCamera_->SetPosition({ 0, 5.5f, -17.0f });
	mainCamera_->SetRotate({ 0.25f, 0.0f, 0.0f });
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {

	if(Input::TriggerKey(KeyCode::R)) {
		SetNextScene("TitleScene");
	}

	Mat4 matScale = Mat4::kIdentity;
	Mat4 matRotate = Mat4::MakeRotateQuaternion(
		Quaternion::Normalize({ 0.0f, -0.7f, 0.0f, 0.7f })
	);


	Mat4 matWorld = matScale * matRotate;

}
