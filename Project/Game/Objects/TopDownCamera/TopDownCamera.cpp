#include "TopDownCamera.h"

/// game
#include "Objects/Camera/GameCamera.h"


TopDownCamera::TopDownCamera(class GameCamera* _gameCamera, BaseGameObject* _targetObject)
	: pGameCamera_(_gameCamera), targetObject_(_targetObject) {

	CreateTag(this);
}

TopDownCamera::~TopDownCamera() {}

void TopDownCamera::Initialize() {
	pGameCamera_->SetParent(pTransform_);
}

void TopDownCamera::Update() {
	pTransform_->position = targetObject_->GetPosition();
}

