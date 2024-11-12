#include "Scene_Result.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/ResultSprite/ResultSprite.h"
#include "Objects/ResultScore/ResultScore.h"


void Scene_Result::Initialize() {

	(new ResultSprite)->Initialize();
	(new ResultScore)->Initialize();

	mainCamera_->SetProjectionType(ORTHOGRAPHIC);
	mainCamera_->SetDistance(10.0f);


}

void Scene_Result::Update() {


}
