#include "Player.h"

/// engine
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Core/Utility/Input/Input.h"


Player::Player() {}
Player::~Player() {}

void Player::Initialize() {

	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshPath("Assets/Models/player.obj");
	renderer->SetTexturePath("Assets/Textures/player.png");

	transform_->scale = Vector3(100.0f, 100.0f, 0.5f); ///< texture size に合わせる

	velocity_ = Vector3::kZero;
	speed_    = 10.0f; //< 移動する速度

}

void Player::Update() {
	
	{	/// 移動処理

		/// WASD keyで移動

		velocity_ = Vector3::kZero;
		
		if (Input::PressKey(DIK_W)) { velocity_.y += 1.0f; }
		if (Input::PressKey(DIK_S)) { velocity_.y -= 1.0f; }
		if (Input::PressKey(DIK_A)) { velocity_.x -= 1.0f; }
		if (Input::PressKey(DIK_D)) { velocity_.x += 1.0f; }

		velocity_ = velocity_.Normalize() * speed_;

		transform_->position += velocity_;

	}

}
