#include "Player.h"

/// extrernal
#include <imgui.h>

/// engine
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Input/Input.h"
#include "FrameManager/Time.h"
#include "Math/LerpShortAngle.h"

/// user
#include "State/PlayerRootState/PlayerRootState.h"
#include "Objects/Camera/GameCamera.h"
#include "Objects/EntityShadow/EntityShadow.h"
#include "PlayerProtectionMesh/PlayerProtectionMesh.h"
#include "Objects/EnemyManager/EnemyManager.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/TargetSpriteRender/TargetSpriteRender.h"


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

	targetSpriteRender_ = new TargetSpriteRender();
	targetSpriteRender_->Initialize();


	/// フラグの初期化
	flags_.resize(PlayerFlag_Max);

	DebugObjectInitialize();
}

void Player::Update() {

	InputUpdate();

	ApplyGravity();
	MeshRotateUpdate();

	UpdateNearEnemy();
	UpdateTargetEnemy();

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

	for(auto& flag : flags_) {
		ImGui::Text("current: %d", flag.currentFlag);
		ImGui::SameLine();
		ImGui::Text("prev: %d", flag.prevFlag);
	}

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
	flags_[PlayerFlag_IsTargetButtonPressed].Set(Input::PressKey(KeyCode::F) || Input::PressPadButton(PadCode::LB));

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





void Player::DebugObjectInitialize() {
#ifdef _DEBUG




#endif // _DEBUG
}



void Player::UpdateTargetEnemy() {


	if(GetFlag(PlayerFlag_IsTargetButtonPressed).Enter()) {
		/// targetがいない場合は一番近い敵をtargetにする
		if(targetEnemy_ == nullptr) {
			targetEnemy_ = nearEnemy_;
			targetSpriteRender_->GetComponent<MeshRenderer>()->isActive = true;
		} else {
			targetEnemy_ = nullptr;
			targetSpriteRender_->GetComponent<MeshRenderer>()->isActive = false;
		}
	}


	if(targetEnemy_ != nullptr) {
		
		targetSpriteRender_->SetPosition(targetEnemy_->GetPosition());

		/*/// targetがいる場合はtargetの方向に向く
		Vec3 targetPosition = targetEnemy_->GetTransform()->position;
		Vec3 playerPosition = pTransform_->position;
		Vec3 targetDirection = (targetPosition - playerPosition).Normalize();
		
		/// カメラの回転に合わせて移動方向を変更
		Mat4 matCameraRotateY = Mat4::MakeRotateY(pGameCamera_->GetRotate().y);
		targetDirection = Mat4::TransformNormal(targetDirection, matCameraRotateY);
		if(targetDirection != Vec3(0.0f, 0.0f, 0.0f)) {
			lastDirection_ = targetDirection;
		}
	
		/// 移動方向に向けて回転
		Vector3 rotate = GetMesh()->GetRotate();
		rotate.y = LerpShortAngle(rotate.y, std::atan2(lastDirection_.x, lastDirection_.z), 0.2f);
		SetMeshRotate(rotate);*/
	}

}

void Player::UpdateNearEnemy() {

	pEnemyList_ = pEnemyManager_->GetEnemyList();

	/// プレイヤーの前方にいる敵だけを取得する
	pForwardEnemyList_.clear();

	{	/// playerのlocal座標に変換して前方にいる敵だけを取得
		Mat4  matPlayerLocalTransform = Mat4::kIdentity;
		Vec3  localPosition = { 0.0f, 0.0f, 0.0f };
	
		for(auto& enemy : pEnemyList_) {
	
			/// playerのlocal座標に変換
			matPlayerLocalTransform = enemy->GetMatTransform() * pTransform_->matTransform.Inverse();
			localPosition = {
				matPlayerLocalTransform.m[3][0],
				matPlayerLocalTransform.m[3][1],
				matPlayerLocalTransform.m[3][2]
			};
	
			/// local座標のzがマイナスなら後ろにいる
			if(localPosition.z < 0.0f) { continue; }

			pForwardEnemyList_.push_back(enemy);
		}
	}


	/// 前方にいる敵のリストを近い順に昇順に並べる
	pForwardEnemyList_.sort([&](const Enemy* _lhs, const Enemy* _rhs) {

		/// local座標に変換する
		Mat4 matPlayerLocalTransformL = _lhs->GetMatTransform() * pTransform_->matTransform.Inverse();
		Mat4 matPlayerLocalTransformR = _rhs->GetMatTransform() * pTransform_->matTransform.Inverse();

		Vec3 localPositionL = {
			matPlayerLocalTransformL.m[3][0],
			matPlayerLocalTransformL.m[3][1],
			matPlayerLocalTransformL.m[3][2]
		};
		Vec3 localPositionR = {
			matPlayerLocalTransformR.m[3][0],
			matPlayerLocalTransformR.m[3][1],
			matPlayerLocalTransformR.m[3][2]
		};

		return localPositionL.Len() < localPositionR.Len();
	});


	/// begin()が一番近い敵
	if(pForwardEnemyList_.size() > 0) {
		nearEnemy_ = *pForwardEnemyList_.begin();
	}

}


