#include "Scene_Result.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/GameResult/GameResult.h"


void Scene_Result::Initialize() {
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

}

void Scene_Result::Update() {


}
