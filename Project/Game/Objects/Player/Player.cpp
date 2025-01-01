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
#include "Objects/Camera/GameCamera.h"
#include "Objects/EntityShadow/EntityShadow.h"
#include "PlayerProtectionMesh/PlayerProtectionMesh.h"


Player::Player(GameCamera* _gameCameraPtr) 
	: pGameCamera_(_gameCameraPtr) {
	CreateTag(this);
}

Player::~Player() {}

void Player::Initialize() {

	playerMesh_ = new PlayerMesh();
	playerMesh_->Initialize();
	playerMesh_->SetParent(pTransform_);
	
	protectionMesh_ = new PlayerProtectionMesh(this);
	protectionMesh_->Initialize();

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

	/// state manager debugging
	ImGui::SeparatorText("/// state manager");
	stateManager_->Debug();


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

	direction_ += {
		Input::GetLeftStick().x,
		0.0f,
		Input::GetLeftStick().y
	};

	direction_ = direction_.Normalize();

	/// カメラの回転に合わせて移動方向を変更
	Mat4 matCameraRotateY = Mat4::MakeRotateY(pGameCamera_->GetRotate().y);
	direction_ = Mat4::TransformNormal(direction_, matCameraRotateY);
	

	if(direction_ != Vec3(0.0f, 0.0f, 0.0f)) {
		lastDirection_ = direction_;
	}



	/// フラグの更新
	flags_[PlayerFlag_IsDush].Set(Input::PressKey(KeyCode::LShift) || Input::PressPadButton(PadCode::RB));
	flags_[PlayerFlag_IsJump].Set(Input::PressKey(KeyCode::Space) || Input::PressPadButton(PadCode::A));
	flags_[PlayerFlag_IsAttack].Set(Input::PressMouse(MouseCode::Left));
	flags_[PlayerFlag_IsProtection].Set(Input::PressMouse(MouseCode::Right));

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


