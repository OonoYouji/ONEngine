#include "Player.h"

/// engine
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Core/Utility/Input/Input.h"

/// user
#include "../EntityConfig/EntityConfig.h"

Player::Player() {}
Player::~Player() {}

void Player::Initialize() {

	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshPath("Assets/Models/entity/player.obj");
	renderer->SetTexturePath("Assets/Textures/uvChecker.png");

	velocity_ = Vector3::kZero;
	speed_    = 1.0f; //< 移動する速度

	type_ = static_cast<int>(Type::white);

}

void Player::Update() {
	
	//{	/// 移動処理

	//	/// WASD keyで移動

	//	velocity_ = Vector3::kZero;
	//	
	//	if (Input::PressKey(DIK_W)) { velocity_.z += 1.0f; }
	//	if (Input::PressKey(DIK_S)) { velocity_.z -= 1.0f; }
	//	if (Input::PressKey(DIK_A)) { velocity_.x -= 1.0f; }
	//	if (Input::PressKey(DIK_D)) { velocity_.x += 1.0f; }

	//	velocity_ = velocity_.Normalize() * speed_;

	//	transform_->position += velocity_;

	//}

}
