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
	
	MeshRenderer* renderer = GetComponent<MeshRenderer>();
	switch (type_) {
	case static_cast<int>(Type::black):
		renderer->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		break;
	case static_cast<int>(Type::white):
		renderer->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		break;
	}


	///!< debug type_ の切り替え
	if (Input::TriggerKey(DIK_SPACE)) {
		if (type_ == static_cast<int>(Type::black)) {
			type_ = static_cast<int>(Type::white);
		} else {
			type_ = static_cast<int>(Type::black);
		}
	}


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
