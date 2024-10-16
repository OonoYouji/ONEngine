#include "Scene_Clear.h"

#include "Objects/Camera/GameCamera.h"
#include "Objects/BossAnimation/BossAnimation.h"

void Scene_Clear::Initialize() {
	(new BossAnimation)->Initialize();

	mainCamera_->SetPosition({ 8.0f, 3.7f, -8.8f });
	mainCamera_->SetRotate({ 0.26f, -0.8f, 0.0f });
	mainCamera_->GetTransform()->rotateOrder = XYZ;
}


void Scene_Clear::Update() {

}
