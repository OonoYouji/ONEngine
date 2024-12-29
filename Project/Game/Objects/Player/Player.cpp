#include "Player.h"

/// extrernal
#include <imgui.h>

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Input/Input.h"
#include "FrameManager/Time.h"
#include "Math/LerpShortAngle.h"

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

	ApplyGravity();
	MeshRotateUpdate();
	stateManager_->Update();

	/// 地面に着地したときの処理
	if(pTransform_->position.y < 0.0f) {
		pTransform_->position.y = 0.0f;
		velocity_.y = 0.0f;
	}

}

void Player::Debug() {

	ImGui::DragFloat3("velocity", &velocity_.x, 0.01f);

	ImGui::DragFloat("gravity accel", &gravityAccel_, 0.01f);

	ImGui::Text("fps : %f", 1.0f / Time::DeltaTime());
	ImGui::Text("delta time : %f", Time::DeltaTime());

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
	if(direction_ != Vec3(0.0f, 0.0f, 0.0f)) {
		lastDirection_ = direction_;
	}

	flags_[PlayerFlag_IsDush].Set(Input::PressKey(KeyCode::LShift));
	flags_[PlayerFlag_IsJump].Set(Input::PressKey(KeyCode::Space));

}

void Player::ApplyGravity() {
	velocity_.y -= gravityAccel_ * Time::DeltaTime();
}

void Player::MeshRotateUpdate() {
	/// 移動方向に向けて回転
	Vector3 rotate = GetMesh()->GetRotate();
	rotate.y = LerpShortAngle(rotate.y, std::atan2(lastDirection_.x, lastDirection_.z), 0.2f);
	SetMeshRotate(rotate);
}


