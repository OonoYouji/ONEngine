#include "Player.h"

/// engine
#include "Input/Input.h"
#include "FrameManager/Time.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// game
#include "PlayerBulletRenderer/PlayerBulletRenderer.h"
#include "PlayerBullet/PlayerBullet.h"


Player::Player() {
	CreateTag(this);
}

Player::~Player() {}

void Player::Initialize() {

	/// render component
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("cube");


	/// create insatnce
	bulletRenderer_ = new PlayerBulletRenderer();
	bulletRenderer_->Initialize();

	/// parameter setting
	direction_      = Vec3::kFront;
	bulletSpeed_    = 10.0f;
	bulletLifeTime_ = 5.0f;
}

void Player::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		Fire();
	}

}


void Player::Fire() {
	PlayerBullet* bullet = new PlayerBullet();
	bullet->Initialize();
	bullet->SetPosition(GetPosition());

	bullet->StartupSetting(
		direction_ * bulletSpeed_ * Time::DeltaTime(),
		bulletLifeTime_
	);

	bulletRenderer_->PushBackBullet(bullet);
}

