#include "Player.h"


/// engine
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"

/// graphics
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"



void Player::Initialize() {
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("Sphere");

}

void Player::Update() {

}

void Player::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {


		ImGui::TreePop();
	}
}

void Player::SetShootingCourse(ShootingCourse* _shootingCourse) {
	pShootingCourse_ = _shootingCourse;
}
