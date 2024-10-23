#define NOMINMAX
#include "Player.h"

/// std
#include <algorithm>
#include <cassert>

/// engine
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"

/// graphics
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/ShootingCourse/ShootingCourse.h"
#include "Objects/PlayerBullet/PlayerBullet.h"

/// math
#include "Math/Quaternion.h"



void Player::Initialize() {
	//auto meshRenderer = AddComponent<MeshRenderer>();
	//meshRenderer->SetModel("axis");

	/// transform
	pTransform_->rotateOrder = XYZ;


	bulletSpeed_ = 100.0f;

}

void Player::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		PlayerBullet* bullet = new PlayerBullet();
		bullet->Initialize();
		bullet->SetPosition(GetPosition());

		Vec3 direction = Mat4::TransformNormal(Vec3::kFront, pTransform_->matTransform);
		Vec3 velocity = direction.Normalize() * (bulletSpeed_ * Time::DeltaTime());
		bullet->SetVelocity(velocity);
	}

}

void Player::Debug() {

	if(ImGui::TreeNodeEx("player param", ImGuiTreeNodeFlags_DefaultOpen)) {

		for(size_t r = 0; r < 4; ++r) {
			for(size_t c = 0; c < 4; ++c) {
				if(c != 0) { ImGui::SameLine(); }
				ImGui::Text("%0.2f, ", pTransform_->matTransform.m[r][c]);
			}
		}

		ImGui::TreePop();
	}

	
	if(ImGui::TreeNodeEx("bullet param", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("speed", &bulletSpeed_, 0.05f);

		ImGui::TreePop();
	}




}



