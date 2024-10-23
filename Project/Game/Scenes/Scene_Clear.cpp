#include "Scene_Clear.h"

#include "Objects/Camera/GameCamera.h"
#include "Objects/DeadEffect/DeadEffect.h"

void Scene_Clear::Initialize() {

	(new DeadEffect)->Initialize();

	mainCamera_->SetPositionZ(-50.0f);
}


void Scene_Clear::Update() {

}
