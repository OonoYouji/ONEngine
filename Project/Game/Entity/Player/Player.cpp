#include "Player.h"

#define NOMINMAX

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"

/// game 
#include "Game/Effects/PlaneEffects/PlayerMoveEffect.h"

Player::Player() {}
Player::~Player() {}

void Player::Initialize() {
	PlayerMoveEffect* walkEffect = pEntityComponentSystem_->GenerateEntity<PlayerMoveEffect>();
	walkEffect->SetParent(this);
}

void Player::Update() {

	Vec3& velo = variables_->Get<Vec3>("velocity");
	float& jumpPower = variables_->Get<float>("jumpPower");
	float& speed = variables_->Get<float>("speed");
	float& gravity = variables_->Get<float>("gravity");
	bool& onGround = variables_->Get<bool>("onGround");

	if (Input::TriggerKey(DIK_R)) {
		transform_->position.y = 0.0f;
		jumpPower = 0.0f;
	}

	velo = Vec3::kZero;

	/// 横移動
	if (Input::PressKey(DIK_W)) { velo.z += 1.0f; }
	if (Input::PressKey(DIK_S)) { velo.z -= 1.0f; }
	if (Input::PressKey(DIK_A)) { velo.x -= 1.0f; }
	if (Input::PressKey(DIK_D)) { velo.x += 1.0f; }

	/// 上下移動
	if (Input::PressKey(DIK_SPACE)) {
		if (onGround) {
			jumpPower = variables_->Get<float>("startJumpPower");
			onGround = false;
		}
	}

	velo = velo.Normalize() * speed;
	velo.y = jumpPower;
	velo = Matrix4x4::Transform(velo, Matrix4x4::MakeRotateY(transform_->rotate.y));

	/// ジャンプ力の減衰
	jumpPower -= gravity * Time::DeltaTime();

	/// 移動
	if (velo != Vector3::kZero) {
		/// 移動
		transform_->position += velo;
	}


	/// 地面に着地したら
	if (ToTerrainCollider* collider = GetComponent<ToTerrainCollider>()) {
		if (collider->GetIsCollided()) {
			//transform_->position.y = 0.0f;
			jumpPower = 0.0f;
			onGround = true;
		}
	}


	if (pCamera_) {
		pCamera_->SetPosition(variables_->Get<Vec3>("cameraOffset"));
	}

}

