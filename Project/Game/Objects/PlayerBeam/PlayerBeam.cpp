#include "PlayerBeam.h"

/// std
#include <format>

#include <imgui.h>

/// engine
#include "Input/Input.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/CapsuleCollider.h"

/// objects
#include "Objects/Reticle/Reticle.h"
#include "Objects/Player/Player.h"


PlayerBeam::PlayerBeam() {
	CreateTag(this);
}

PlayerBeam::~PlayerBeam() {}

void PlayerBeam::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Beam");

    capsuleCollider_ =  AddComponent<CapsuleCollider>();

	pTransform_->rotateOrder = QUATERNION;
	pTransform_->position.x  = -1.0f;

	capsuleCollider_->SetPositionArray({ &colliderPositionArray_[0], &colliderPositionArray_[1] });
}

void PlayerBeam::Update() {
	meshRenderer_->isActive = false;
	if(Input::PressKey(KeyCode::Space)) {
		meshRenderer_->isActive = true;

		diff_ = pReticle_->GetTransform()->position - pTransform_->position;

		pTransform_->quaternion = Quaternion::LockAt({}, diff_.Normalize());
		pTransform_->scale.z = diff_.Len();
	
		colliderPositionArray_[0] = {};
		colliderPositionArray_[1] = diff_;
	}
}

void PlayerBeam::Debug() {
	ImGui::DragFloat3("diff", &diff_.x);
}

void PlayerBeam::SetReticle(Reticle* _reticle) {
	pReticle_ = _reticle;
}

void PlayerBeam::SetPlayer(Player* _player) {
	pPlayer_ = _player;
}

