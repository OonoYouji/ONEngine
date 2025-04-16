#include "Player.h"

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Input/Input.h"

Player::Player() {}
Player::~Player() {}

void Player::Initialize() {
	//transform_->position = { 0.0f, 0.0f, -10.0f };
	
	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMeshPath("Assets/Models/entity/player.obj");
	meshRenderer->SetTexturePath("Assets/Textures/uvChecker.png");

	AddComponent<Variables>();

	speed_ = 10.0f;

}

void Player::Update() {

	velocity_ = Vector3::kZero;

	/// 横移動
	if (Input::PressKey(DIK_UP)) { velocity_.z += 1.0f; }
	if (Input::PressKey(DIK_DOWN)) { velocity_.z -= 1.0f; }
	if (Input::PressKey(DIK_LEFT)) { velocity_.x -= 1.0f; }
	if (Input::PressKey(DIK_RIGHT)) { velocity_.x += 1.0f; }

	/// 上下移動
	if (Input::PressKey(DIK_SPACE)) { 
		if (jumpPower_ <= 0.0f) {
			jumpPower_ = 10.0f;
		}
	}

	velocity_ = velocity_.Normalize() * speed_;
	velocity_.y = jumpPower_;
	velocity_ = Matrix4x4::Transform(velocity_, Matrix4x4::MakeRotateY(transform_->rotate.y));

	/// ジャンプ力の減衰
	jumpPower_ -= 0.1f;

	/// 移動
	if (velocity_ != Vector3::kZero) {
		/// 移動
		transform_->position += velocity_;
	}


	/// 地面に着地したら
	if (transform_->position.y < 0.0f) {
		transform_->position.y = 0.0f;
		jumpPower_ = 0.0f;
	}


}

