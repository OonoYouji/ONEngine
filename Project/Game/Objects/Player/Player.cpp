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
#include "Objects/Reticle/Reticle.h"
#include "Objects/Score/Score.h"
#include "Objects/DefeatedEnemy/DefeatedEnemy.h"

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

		/// 発射ゲージがあれば
		if(firingGauge_ > firingSubValue_) {

			firingGauge_ -= firingSubValue_;
			firingGauge_ = std::max(firingGauge_, 0.0f); /// 0未満にならないように

			PlayerBullet* bullet = new PlayerBullet(this, pDefeatedEnemy_);
			bullet->Initialize();
			bullet->SetPosition(GetPosition());

			Vec3 diff = pReticle_->GetPosition() - GetPosition();
			Vec3 velocity = diff.Normalize() * (bulletSpeed_ * Time::DeltaTime());
			bullet->SetVelocity(velocity);
		}  

	} else {

		firingGauge_ += firingAddValue_;
		firingGauge_ = std::min(firingGauge_, 100.0f); /// 100を超過しないように
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

void Player::SetReticle(Reticle* _reticle) {
	pReticle_ = _reticle;
}

void Player::AddScore(uint32_t _value) {
	pScore_->AddScore(_value);
}

void Player::SetScoreObj(Score* _score) {
	pScore_ = _score;
}

void Player::SetDefeatedEnemy(DefeatedEnemy* _defeatedEnemy) {
	pDefeatedEnemy_ = _defeatedEnemy;
}



