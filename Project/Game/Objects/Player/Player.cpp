#include "Player.h"

/// std
#include <algorithm>

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
#include "PlayerAttackCollider/PlayerAttackCollider.h"
#include "Objects/Player/PlayerMoveLocus/PlayerMoveLocus.h"


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

	targetSpriteRender_ = new TargetSpriteRender(pGameCamera_);
	targetSpriteRender_->Initialize();

	attackCollider_ = new PlayerAttackCollider();
	attackCollider_->Initialize();

	moveLocus_ = new PlayerMoveLocus(this);
	moveLocus_->Initialize();
	moveLocus_->SetParent(pTransform_);

	/// フラグの初期化
	flags_.resize(PlayerFlag_Max);

	DebugObjectInitialize();
}

void Player::Update() {

	InputUpdate();

	ApplyGravity();
	MeshUpdate();

	UpdateNearEnemy();
	UpdateTargetEnemy();

	stateManager_->Update();

	/// 地面に着地したときの処理
	if(pTransform_->position.y < 0.0f) {
		pTransform_->position.y = 0.0f;
		velocity_.y = 0.0f;
	}

	PushBackStage();

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
	flags_[PlayerFlag_IsAttack].Set(Input::PressMouse(MouseCode::Left) || Input::PressPadButton(PadCode::X));
	//flags_[PlayerFlag_IsProtection].Set(Input::PressKey(KeyCode::LeftControl));
	flags_[PlayerFlag_IsTargetButtonPressed].Set(Input::PressKey(KeyCode::F) || Input::PressPadButton(PadCode::LB));
	flags_[PlayerFlag_IsTarget].Set(targetEnemy_ != nullptr);

	/// 左右キーの入力フラグ
	flags_[PlayerFlag_InputRight].Set(Input::PressKey(KeyCode::RightArrow) || Input::PressPadButton(PadCode::Right));
	flags_[PlayerFlag_InputLeft].Set(Input::PressKey(KeyCode::LeftArrow) || Input::PressPadButton(PadCode::Left));


}

void Player::ApplyGravity() {
	velocity_.y -= gravityAccel_ * Time::DeltaTime();
}

void Player::MeshUpdate() {
	/// 移動方向に向けて回転
	Vec3 rotate = GetMesh()->GetRotate();
	rotate.y = LerpShortAngle(rotate.y, std::atan2(lastDirection_.x, lastDirection_.z), 0.2f);
	SetMeshRotate(rotate);

	if(stateManager_->GetCurrentStateIndex() == PlayerStateOrder_Dush) {
		currentFovY_ = std::lerp(currentFovY_, 0.6f, 0.1f);
	} else {
		currentFovY_ = std::lerp(currentFovY_, 0.45f, 0.1f);
	}

	pGameCamera_->SetFovY(currentFovY_);
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
		
		/// 
		if(GetFlag(PlayerFlag_InputLeft).Enter()) {
			nearEnemyIndex_--;
			if(nearEnemyIndex_ < 0) {
				nearEnemyIndex_ = static_cast<int>(pForwardEnemyList_.size() - 1);
			}
		}
		if(GetFlag(PlayerFlag_InputRight).Enter()) {
			nearEnemyIndex_++;
			if(nearEnemyIndex_ >= pForwardEnemyList_.size()) {
				nearEnemyIndex_ = 0;
			}
		}

		if(nearEnemyIndex_ > 0 && nearEnemyIndex_ < pForwardEnemyList_.size()) {
			targetEnemy_ = *std::next(pForwardEnemyList_.begin(), nearEnemyIndex_);
		}


		targetSpriteRender_->SetPosition(targetEnemy_->GetPosition());

		/// TODO: targetが画面の後ろに出たらtargetを外す

	}

}

void Player::UpdateNearEnemy() {

	pEnemyList_ = pEnemyManager_->GetEnemyList();

	/// プレイヤーの前方にいる敵だけを取得する
	pForwardEnemyList_.clear();

	{	/// camaraのlocal座標に変換して前方にいる敵だけを取得
		Mat4  matCameraLocalTransform = Mat4::kIdentity;
		Vec3  localPosition = { 0.0f, 0.0f, 0.0f };
	
		for(auto& enemy : pEnemyList_) {
	
			/// playerのlocal座標に変換
			matCameraLocalTransform = enemy->GetMatTransform() * pGameCamera_->GetMatTransform().Inverse();
			localPosition = {
				matCameraLocalTransform.m[3][0],
				matCameraLocalTransform.m[3][1],
				matCameraLocalTransform.m[3][2]
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

void Player::PushBackStage() {
	Vec3 minSize = { -65.0f, 0.0f, -65.0f };
	Vec3 maxSize = {  65.0f, 0.0f,  65.0f };

	Vec3& position = pTransform_->position;
	position.x = std::clamp(position.x, minSize.x, maxSize.x);
	position.z = std::clamp(position.z, minSize.z, maxSize.z);

}


