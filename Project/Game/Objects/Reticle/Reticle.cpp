#include "Reticle.h"

/// std
#include <cassert>

/// engine
#include <imgui.h>
#include "Input/Input.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/Camera/GameCamera.h"


Reticle::Reticle() {
	CreateTag(this);
}

Reticle::~Reticle() {}

void Reticle::Initialize() {

	/// component
	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("axis");

	/// transform
	pTransform_->position = { 0, 0, 10.0f };
	pTransform_->scale *= 0.5f;

	speed_ = 0.1f;

}

void Reticle::Update() {
	
	velocity_ = { 0,0,0 };
	if(Input::PressKey(KeyCode::W)) { velocity_.y += 1.0f;}
	if(Input::PressKey(KeyCode::A)) { velocity_.x -= 1.0f;}
	if(Input::PressKey(KeyCode::S)) { velocity_.y -= 1.0f;}
	if(Input::PressKey(KeyCode::D)) { velocity_.x += 1.0f;}

	velocity_ = velocity_.Normalize() * speed_;

	/// 移動制限
	const Vec3& position = pTransform_->position;
	Vec3 nextPos = position + velocity_;
	if(nextPos.x < -3.25f || nextPos.x > 3.25f) {
		velocity_.x = 0.0f;
	}

	if(std::abs(nextPos.y) > 1.7f) {
		velocity_.y = 0.0f;
	}

	pTransform_->position += velocity_;

}

void Reticle::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("velocity", &velocity_.x, 0.05f);
		ImGui::DragFloat("speed",     &speed_,      0.05f);

		ImGui::TreePop();
	}
}



void Reticle::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
	assert(pGameCamera_);

	SetParent(pGameCamera_->GetTransform());

}
