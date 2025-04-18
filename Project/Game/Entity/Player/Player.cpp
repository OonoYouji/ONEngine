#include "Player.h"

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Input/Input.h"

Player::Player() {}
Player::~Player() {}

void Player::Initialize() {
	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMeshPath("Assets/Models/entity/player.obj");
	meshRenderer->SetTexturePath("Packages/Textures/uvChecker.png");

	Effect* effect = AddComponent<Effect>();
	//effect->modelPath_ = "Assets/Models/entity/player.obj";

}

void Player::Update() {

	Vec3& velo = variables_->Get<Vec3>("velocity");
	float& jumpPower = variables_->Get<float>("jumpPower");
	float& speed = variables_->Get<float>("speed");

	velo = Vec3::kZero;

	/// 横移動
	if (Input::PressKey(DIK_UP)) { velo.z += 1.0f; }
	if (Input::PressKey(DIK_DOWN)) { velo.z -= 1.0f; }
	if (Input::PressKey(DIK_LEFT)) { velo.x -= 1.0f; }
	if (Input::PressKey(DIK_RIGHT)) { velo.x += 1.0f; }

	/// 上下移動
	if (Input::PressKey(DIK_SPACE)) { 
		if (jumpPower <= 0.0f) {
			jumpPower = 10.0f;
		}
	}

	velo = velo.Normalize() * speed;
	velo.y = jumpPower;
	velo = Matrix4x4::Transform(velo, Matrix4x4::MakeRotateY(transform_->rotate.y));

	/// ジャンプ力の減衰
	jumpPower -= 0.1f;

	/// 移動
	if (velo != Vector3::kZero) {
		/// 移動
		transform_->position += velo;
	}


	/// 地面に着地したら
	if (transform_->position.y < 0.0f) {
		transform_->position.y = 0.0f;
		jumpPower = 0.0f;
	}


}

