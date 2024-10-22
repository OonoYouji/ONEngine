#include "PlayerBullet.h"

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


PlayerBullet::PlayerBullet() {
	CreateTag(this);
}

PlayerBullet::~PlayerBullet() {}

void PlayerBullet::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Sphere");
}

void PlayerBullet::Update() {
	pTransform_->position += velocity_;

	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ < 0.0f) {
		Destory();
	}
}

void PlayerBullet::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("velocity", &velocity_.x, 0.05f);

		ImGui::TreePop();
	}
}

void PlayerBullet::SetVelocity(const Vec3& _velocity) {
	velocity_ = _velocity;
}
