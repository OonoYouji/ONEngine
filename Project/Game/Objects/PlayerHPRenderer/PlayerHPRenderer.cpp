#include "PlayerHPRenderer.h"

/// externals
#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"



PlayerHPRenderer::PlayerHPRenderer() {
	CreateTag(this);
}

PlayerHPRenderer::~PlayerHPRenderer() {}

void PlayerHPRenderer::Initialize() {

	/// ui layerに表示
	drawLayerId = 1;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("white2x2");

	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
	vm->AddValue(groupName, "rotate", pTransform_->rotate);

	/// jsonの読み込み
	vm->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);

	ApplyVariables();

}

void PlayerHPRenderer::Update() {

}

void PlayerHPRenderer::Debug() {
	if(ImGui::TreeNodeEx("label", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::Button("apply variable")) {
			ApplyVariables();
		}
		
		if(ImGui::Button("set variable")) {
			SetVariables();
		}

		ImGui::TreePop();
	}
}


void PlayerHPRenderer::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
	pTransform_->rotate   = vm->GetValue<Vec3>(groupName, "rotate");
}


void PlayerHPRenderer::SetVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->SetValue(groupName, "position", pTransform_->position);
	vm->SetValue(groupName, "scale",    pTransform_->scale);
	vm->SetValue(groupName, "rotate",   pTransform_->rotate);
}