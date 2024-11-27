#define NOMINMAX
#include "PlayerDamageEffect.h"

#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "Scenes/Scene_Game.h"


PlayerDamageEffect::PlayerDamageEffect() {
	CreateTag(this);
}

PlayerDamageEffect::~PlayerDamageEffect() {}

void PlayerDamageEffect::Initialize() {

	drawLayerId = GAME_SCENE_LAYER_UI;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("playerDamage.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);

}

void PlayerDamageEffect::Update() {
	ApplyVariables();

	currentTime_ += Time::DeltaTime();

	float lerpT = std::min(currentTime_ / maxTime_, 1.0f);
	alpha_ = std::lerp(maxAlpha_, 0.0f, lerpT);

	spriteRenderer_->SetColor({1,1,1, alpha_});
}

void PlayerDamageEffect::Debug() {

	{
		static float time, damage;
		ImGui::DragFloat("time", &time, 0.1f);
		ImGui::DragFloat("damage", &damage, 0.1f);

		if(ImGui::Button("start")) {
			Start(time, damage);
		}
	}



	ImGui::SliderFloat("alpha", &alpha_, 0.0f, 1.0f);
}

void PlayerDamageEffect::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale",    pTransform_->scale);
	vm->AddValue(groupName, "maxDamageEffect", maxDamageEffect_);
	vm->AddValue(groupName, "minDamageEffect", minDamageEffect_);

}

void PlayerDamageEffect::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
	maxDamageEffect_      = vm->GetValue<float>(groupName, "maxDamageEffect");
	minDamageEffect_      = vm->GetValue<float>(groupName, "minDamageEffect");

}

void PlayerDamageEffect::Start(float _time, float _damage) {

	float damage = std::clamp(_damage, minDamageEffect_, maxDamageEffect_);
	maxAlpha_ = damage / maxDamageEffect_;

	maxTime_ = _time;
	currentTime_ = 0.0f;

}

