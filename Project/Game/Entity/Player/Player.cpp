#include "Player.h"

/// engine
#include "Engine/Core/Utility/Input/Input.h"
#include "Engine/ECS/Component/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/ComputeComponents/Light/Light.h"

/// user
#include "../EntityConfig/EntityConfig.h"

Player::Player() {}
Player::~Player() {}

void Player::Initialize() {

	DirectionalLight* light = AddComponent<DirectionalLight>();
	light->SetIntensity(1.0f);
	light->SetDirection(Vector3(0.0f, -1.0f, 0.0f));
	light->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));


	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshPath("Assets/Models/entity/player.obj");
	renderer->SetTexturePath("Assets/Textures/uvChecker.png");

	/// 適当な値で初期化
	type_ = static_cast<int>(Type::white);

	currentAddress_ = { 0, 0 };
	nextAddress_    = { 0, 0 };
	isAnimation_    = false;
	isMove_         = false;

}

void Player::Update() {
	
#ifdef _DEBUG
	///!< debug type_ の切り替え
	if (Input::TriggerKey(DIK_SPACE)) {
		if (type_ == static_cast<int>(Type::black)) {
			type_ = static_cast<int>(Type::white);
		} else {
			type_ = static_cast<int>(Type::black);
		}
	}
#endif // _DEBUG



	MeshRenderer* renderer = GetComponent<MeshRenderer>();
	switch (type_) {
	case static_cast<int>(Type::black):
		renderer->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		break;
	case static_cast<int>(Type::white):
		renderer->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		break;
	}


	transform_->position = Vector3(
		static_cast<float>(nextAddress_.col) * GameConfig::kBlockSize,
		1.0f, 
		static_cast<float>(nextAddress_.row) * GameConfig::kBlockSize
	);


	if (isMove_) {
		currentAddress_ = nextAddress_;
		isMove_         = false;
	}

}

void Player::Move(const Address& _address) {

	nextAddress_     = _address;
	isMove_          = true;
	isAnimation_     = true;
	currentMoveTime_ = 0.0f;
}
