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
	//pCamera_ = pEntityComponentSystem_->GenerateCamera<Camera>();
	//if (pCamera_) {
	//	pCamera_->SetParent(this);
	//	pEntityComponentSystem_->SetMainCamera(pCamera_);
	//}
	
	Script* script = AddComponent<Script>();
	script->SetScript("Player");

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

	Vector2 gamepadLeftThumb = Input::GetGamepadLeftThumb();
	if (gamepadLeftThumb.x != 0.0f || gamepadLeftThumb.y != 0.0f) {
		velo.x += gamepadLeftThumb.x;
		velo.z += gamepadLeftThumb.y;
	}

	/// 上下移動
	if (Input::PressGamepad(Gamepad::A)) {
		if (onGround) {
			jumpPower = variables_->Get<float>("startJumpPower");
			onGround = false;
		}
	}


	/// 回転
	Vector3& cameraRotate = variables_->Get<Vector3>("cameraRotate");
	float& cameraRotateRatio = variables_->Get<float>("cameraRotateRatio");

	/// カメラの回転
	Vector2 gamepadRightThumb = Input::GetGamepadRightThumb();
	if (gamepadRightThumb.x != 0.0f || gamepadRightThumb.y != 0.0f) {
		transform_->rotate.y += gamepadRightThumb.x * cameraRotateRatio;
		cameraRotate.x += gamepadRightThumb.y * cameraRotateRatio;
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


		pCamera_->SetPosition(variables_->Get<Vector3>("cameraOffset"));
	}

}

