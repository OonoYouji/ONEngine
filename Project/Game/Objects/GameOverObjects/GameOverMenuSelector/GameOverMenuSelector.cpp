#include "GameOverMenuSelector.h"

#include "Input/Input.h"
#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "../GameOverToRestartText/GameOverToRestartText.h"
#include "../GameOverToTitleText/GameOverToTitleText.h"

#include "Objects/GameManagerObject/GameManagerObject.h"

#include "Scenes/Scene_Result.h"

GameOverMenuSelector::GameOverMenuSelector() {
	CreateTag(this);
}

GameOverMenuSelector::~GameOverMenuSelector() {}

void GameOverMenuSelector::Initialize() {
	drawLayerId = RESULT_LAYER_UI;

	if(GameManagerObject::GetFlag("isEnemyHalfLife").Press()) {
		texts_[0] = new GameOverToRestartText();
	} else {
		texts_[0] = nullptr;
	}

	texts_[1] = new GameOverToTitleText();

	for(auto& text : texts_) {
		if(text) {
			text->Initialize();
			text->drawLayerId = drawLayerId;
		}
	}

	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("Arrow.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();
}

void GameOverMenuSelector::Update() {
	ApplyVariables();

	bool isUp = false;
	bool isDown = false;

	isUp |= Input::TriggerKey(KeyCode::D);
	isUp |= Input::TriggerKey(KeyCode::ArrowRight);
	isUp |= Input::TriggerPadButton(PadCode::Right);
	isUp |= Input::IsLStickStartMoving() && Input::GetLeftStick().x > 0.0f;

	isDown |= Input::TriggerKey(KeyCode::A);
	isDown |= Input::TriggerKey(KeyCode::ArrowLeft);
	isDown |= Input::TriggerPadButton(PadCode::Left);
	isDown |= Input::IsLStickStartMoving() && Input::GetLeftStick().x > 0.0f;


	if(isUp) { ++selectMenuIndex_; }
	if(GameManagerObject::GetFlag("isEnemyHalfLife").Press()) {
		if(isDown) { --selectMenuIndex_; }
	} else {
		selectMenuIndex_ = 1;
	}

	selectMenuIndex_ = std::clamp(selectMenuIndex_, 0, 1);
	pTransform_->position = texts_[selectMenuIndex_]->GetPosition();
	pTransform_->rotate = texts_[selectMenuIndex_]->GetRotate();
	//pTransform_->scale = texts_[selectMenuIndex_]->GetScale();

}



void GameOverMenuSelector::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void GameOverMenuSelector::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}