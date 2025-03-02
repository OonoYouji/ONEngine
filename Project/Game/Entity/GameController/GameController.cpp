#include "GameController.h"

/// std
#include <numbers>

/// engine
#include <Engine/Entity/Collection/EntityCollection.h>
#include "Engine/Entity/Camera/Camera.h"

/// user
#include "../Player/Player.h"
#include "../Block/Block.h"


GameController::GameController() {}
GameController::~GameController() {}



void GameController::Initialize() {

	Camera* camera = entityCollection_->GenerateCamera();
	camera->SetPosition(Vector3(0.0f, 20.0f, -25.0f));
	camera->SetRotate(Vector3(std::numbers::pi_v<float> / 5.0f, 0.0f, 0.0f));

	entityCollection_->GenerateEntity<Player>();
	entityCollection_->GenerateEntity<BlockManager>();
}

void GameController::Update() {}
