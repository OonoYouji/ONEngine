#include "Scene_Result.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/GameResult/GameResult.h"

#include "Objects/Tornado/Tornado.h"

void Scene_Result::Initialize() {

	/// reset
	Ring::ResetInstanceCount();
	Wind::ResetInstanceCount();


	/// instance create
	GameCamera* spriteCamera = new GameCamera("spriteCamera");
	GameResult* gameResult   = new GameResult();

	/// initializing
	spriteCamera->Initialize();
	gameResult->Initialize();

	/// other setting
	spriteCamera->SetProjectionType(ORTHOGRAPHIC);
	spriteCamera->SetDistance(6.0f);
	AddLayer("sprite", spriteCamera);


	mainCamera_->GetTransform()->rotateOrder = XYZ;
	mainCamera_->SetPosition({ -10.0f, 5.5f, -15.725f });
	mainCamera_->SetRotate({ 0.308f, 0.8f, 0.0f });


}

void Scene_Result::Update() {


}
