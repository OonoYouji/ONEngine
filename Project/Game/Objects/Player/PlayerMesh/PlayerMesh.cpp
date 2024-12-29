#include "PlayerMesh.h"

/// external
#include <imgui.h>

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "FrameManager/Time.h"
#include "VariableManager/VariableManager.h"

/// user
#include "Objects/EntityShadow/EntityShadow.h"


PlayerMesh::PlayerMesh() {
	CreateTag(this);
}

PlayerMesh::~PlayerMesh() {}

void PlayerMesh::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Player");

	/// 影の生成
	EntityShadow* shadow = new EntityShadow(this, GetModel());
	shadow->Initialize();


	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);
	ApplyVariables();
}

void PlayerMesh::Update() {

	/// デバッグ
	ApplyVariables();


	animationTime_ += Time::DeltaTime();

	Vec3 position = offsetPosition_ + Vec3(0, animationRange_ * std::sin(animationTime_ * animationSpeed_), 0);
	pTransform_->position = position;

}

void PlayerMesh::Debug() {
	ImGui::DragFloat3("offset position", &offsetPosition_.x, 0.05f);
	ImGui::DragFloat("animation speed", &animationSpeed_, 0.05f);
	ImGui::DragFloat("animation range", &animationRange_, 0.05f);
}

void PlayerMesh::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "offsetPosition", offsetPosition_);
	vm->AddValue(groupName, "animationSpeed", animationSpeed_);
	vm->AddValue(groupName, "animationRange", animationRange_);
}

void PlayerMesh::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	offsetPosition_ = vm->GetValue<Vec3>(groupName, "offsetPosition");
	animationRange_ = vm->GetValue<float>(groupName, "animationRange");
	animationSpeed_ = vm->GetValue<float>(groupName, "animationSpeed");
}

Model* PlayerMesh::GetModel() const {
	return meshRenderer_->GetModel();
}

