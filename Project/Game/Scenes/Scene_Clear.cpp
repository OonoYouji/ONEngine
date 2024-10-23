#include "Scene_Clear.h"

#include "Objects/Camera/GameCamera.h"
#include "Objects/DeadEffect/DeadEffect.h"
#include "Objects/BossAnimation/BossAnimation.h"

void Scene_Clear::Initialize() {
	(new BossAnimation)->Initialize();
	
	DeadEffect* deadEffect = (new DeadEffect);
	deadEffect->Initialize();
	deadEffect->drawLayerId = 1;

	GameCamera* particleCamera = new GameCamera("particle camera");
	particleCamera->Initialize();
	particleCamera->SetPosition({0,0,-6.6f});

	mainCamera_->SetPositionZ(-50.0f);


	AddLayer("particle", particleCamera);

}


void Scene_Clear::Update() {

}
