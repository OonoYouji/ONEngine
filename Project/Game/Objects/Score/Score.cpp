#include "Score.h"

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/NumberRenderer/NumberRenderer.h"

/// scenes
#include "Scenes/Scene_Game.h"


Score::Score() {
	CreateTag(this);
}

Score::~Score() {}

void Score::Initialize() {

	drawLayerId = SCENE_GAME_LAYER_UI;

	numberRenderer_ = AddComponent<NumberRenderer>(4u);


	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale",    pTransform_->scale);


}

void Score::Update() {
	ApplyVariables();

}

void Score::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
}

