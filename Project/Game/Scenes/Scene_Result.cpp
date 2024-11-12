#include "Scene_Result.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/ResultSprite/ResultSprite.h"


void Scene_Result::Initialize() {

	(new ResultSprite)->Initialize();

	mainCamera_->SetProjectionType(ORTHOGRAPHIC);
	mainCamera_->SetDistance(10.0f);


}

void Scene_Result::Update() {


}
