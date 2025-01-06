#include "PlayerProtectionMesh.h"

/// external
#include <imgui.h>

/// engine
#include "ComponentManager/Collider/BoxCollider.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "VariableManager/VariableManager.h"
#include "GraphicManager/ModelManager/ModelManager.h"

/// user
#include "../Player.h"

PlayerProtectionMesh::PlayerProtectionMesh(Player* _playerPtr)
	: pPlayer_(_playerPtr) {
	CreateTag(this);
}

PlayerProtectionMesh::~PlayerProtectionMesh() {}

void PlayerProtectionMesh::Initialize() {\

	Model* model = ModelManager::Load("PlayerProtectionMesh");

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);
	meshRenderer_->SetMaterial("white2x2.png");
	meshRenderer_->SetColor(Vec4(120, 225, 120, 190) / 255.0f);

	SetParent(pPlayer_->GetMesh()->GetTransform());

	boxCollider_ = AddComponent<BoxCollider>(model);


	pTransform_->position.z = 2.0f;


	AddVariables();
	//LoadVariables();
	ApplyVariables();
}

void PlayerProtectionMesh::Update() {

	meshRenderer_->isActive = false;
	if(pPlayer_->GetFlag(PlayerFlag_IsProtection).Stay()) {
		meshRenderer_->isActive = true;
	}

}

void PlayerProtectionMesh::Debug() {
	
	if(ImGui::Button("reload json")) {
		LoadVariables();
	}
 
}



void PlayerProtectionMesh::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
}

void PlayerProtectionMesh::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
}

void PlayerProtectionMesh::LoadVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string groupName = GetTag();

	vm->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects/",
		groupName
	);
}
