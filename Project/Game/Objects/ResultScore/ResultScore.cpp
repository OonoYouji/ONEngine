#include "ResultScore.h"

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/NumberRenderer/NumberRenderer.h"

/// score
#include "Objects/Score/Score.h"

ResultScore::ResultScore() {
	CreateTag(this);
}

ResultScore::~ResultScore() {}

void ResultScore::Initialize() {

	uint32_t score      = Score::GetGameResultScore();
	uint32_t scoreDigit = static_cast<uint32_t>(std::to_string(score).length());

	numberRenderer_ = AddComponent<NumberRenderer>(scoreDigit);
	numberRenderer_->SetTexture("number.png");

	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale",    pTransform_->scale);


	ApplyVariables();


	numberRenderer_->SetScore(score);

}

void ResultScore::Update() {

}

void ResultScore::ApplyVariables() {

	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");

}

