#include "Player.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Input/Input.h"

/// user
#include "State/PlayerRootState/PlayerRootState.h"

Player::Player(GameCamera* _gameCameraPtr) {
	CreateTag(this);
}

Player::~Player() {}

void Player::Initialize() {

	playerMesh_ = new PlayerMesh();
	playerMesh_->Initialize();
	playerMesh_->SetParent(pTransform_);


	stateManager_.reset(new PlayerStateManager(this));
	stateManager_->Initialize();

	/// フラグの初期化
	flags_.resize(PlayerFlag_Max);

}

void Player::Update() {

	InputUpdate();

	stateManager_->Update();

}

void Player::InputUpdate() {

	/// すべてのフラグを更新
	for(auto& flag : flags_) {
		flag.Update();
	}

	/// 方向の初期化
	direction_ = { 0.0f, 0.0f, 0.0f };

	if(Input::PressKey(KeyCode::W)) { direction_.z += 1.0f; }
	if(Input::PressKey(KeyCode::A)) { direction_.x -= 1.0f; }
	if(Input::PressKey(KeyCode::S)) { direction_.z -= 1.0f; }
	if(Input::PressKey(KeyCode::D)) { direction_.x += 1.0f; }

	direction_ = direction_.Normalize();

	flags_[PlayerFlag_IsDush].Set(Input::PressKey(KeyCode::LShift));

}

