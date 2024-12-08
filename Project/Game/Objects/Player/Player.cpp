#include "Player.h"

/// engine
#include "Input/Input.h"
#include "FrameManager/Time.h"
#include "VariableManager/VariableManager.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/BoxCollider.h"
#include "ComponentManager/Collider/SphereCollider.h"

/// game
#include "PlayerBulletRenderer/PlayerBulletRenderer.h"
#include "PlayerBullet/PlayerBullet.h"
#include "Objects/Effect/PlayerNormalEffect/PlayerNormalEffect.h"



Player::Player() {
	CreateTag(this);
}

Player::~Player() {}

void Player::Initialize() {

	Model* model = ModelManager::Load("cube");

	/// render component
	SphereCollider_ = AddComponent<SphereCollider>(model);

	
	/// render component
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);


	/// create insatnce
	bulletRenderer_ = new PlayerBulletRenderer();
	bulletRenderer_->Initialize();

	normalEffect_   = new PlayerNormalEffect(this);
	normalEffect_->Initialize();
	normalEffect_->SetParent(pTransform_);

	/// parameter setting
	direction_      = Vec3::kFront;
	bulletSpeed_    = 10.0f;
	bulletLifeTime_ = 5.0f;



	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);
	ApplyVariables();
}

void Player::Update() {
	ApplyVariables();

	Movement();
	pTransform_->Update();

	if(Input::TriggerKey(KeyCode::Space)) {
		Fire();
	}

}

void Player::OnCollisionEnter(BaseGameObject* const _collision) {

}

void Player::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "bulletSpeed",    bulletSpeed_);
	vm->AddValue(groupName, "bulletLifeTime", bulletLifeTime_);
	vm->AddValue(groupName, "movementSpeed",  movementSpeed_);
}

void Player::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	bulletSpeed_    = vm->GetValue<float>(groupName, "bulletSpeed");
	bulletLifeTime_ = vm->GetValue<float>(groupName, "bulletLifeTime");
	movementSpeed_  = vm->GetValue<float>(groupName, "movementSpeed");
}



void Player::Movement() {

	velocity_ = {};
	if(Input::PressKey(KeyCode::W)) { velocity_.z += 1.0f; }
	if(Input::PressKey(KeyCode::A)) { velocity_.x -= 1.0f; }
	if(Input::PressKey(KeyCode::S)) { velocity_.z -= 1.0f; }
	if(Input::PressKey(KeyCode::D)) { velocity_.x += 1.0f; }

	velocity_ = velocity_.Normalize() * movementSpeed_;

	pTransform_->position += velocity_ * Time::DeltaTime();
}


void Player::Fire() {
	PlayerBullet* bullet = new PlayerBullet(pTopDownCamera_);
	bullet->Initialize();
	bullet->SetPosition(GetPosition());

	bullet->StartupSetting(
		direction_ * bulletSpeed_ * Time::DeltaTime(),
		bulletLifeTime_
	);

	bulletRenderer_->PushBackBullet(bullet);
}

void Player::SetTopDownCamera(TopDownCamera* _camera) {
	pTopDownCamera_ = _camera;
}

