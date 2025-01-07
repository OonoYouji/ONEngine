#include "PlayerNormalAttackStep2.h"

/// std
#include <algorithm>

/// external
#include <json.hpp>
#include <imgui.h>

/// engine
#include "FrameManager/Time.h"

/// user
#include "../../Player.h"
#include "../../PlayerAttackCollider/PlayerAttackCollider.h"

using namespace nlohmann;


PlayerNormalAttackStep2::PlayerNormalAttackStep2(Player* _player, PlayerStateManager* _stateManager) 
	: IPlayerState(_player, _stateManager) {

	defaultAttackPos_[0] = Vec3( 0.0f, 0.0f,  0.0f);
	defaultAttackPos_[1] = Vec3(-4.0f, 0.0f, -3.0f);
	defaultAttackPos_[2] = Vec3( 4.0f, 0.0f, -6.0f);
	defaultAttackPos_[3] = Vec3( 0.0f, 0.0f, -9.0f);

}

PlayerNormalAttackStep2::~PlayerNormalAttackStep2() {}


void PlayerNormalAttackStep2::Start() {
	
	startPosition_ = pPlayer_->GetPosition();

	Mat4 matRotate = Mat4::MakeRotateY(pPlayer_->GetMesh()->GetRotate().y);
	for(int i = 0; i < 4; ++i) {
		attackPos_[i] = Mat4::Transform(defaultAttackPos_[i], matRotate);
		attackPos_[i] += startPosition_;
	}

	PlayerAttackCollider* collider = pPlayer_->GetAttackCollider();
	collider->SetPosition((attackPos_[0] + attackPos_[3]) * 0.5f);
	collider->SetRotate(Mat4::TransformNormal({}, matRotate));
	collider->SetScale(Vec3(5.0f, 2.0f, 5.0f));
	collider->UpdateMatrix();
	collider->SetColliderActive(true);


	maxTime_     = 0.2f;
	currentTime_ = 0.0f;


	isEnd_ = false;

}

void PlayerNormalAttackStep2::Update() {

	const float kMaxT = 3.0f;

	currentTime_ += kMaxT * Time::DeltaTime();
	float lerpT = std::clamp((currentTime_ / maxTime_) / kMaxT, 0.0f, kMaxT);

	Vec3 position = Vec3ArrayLerp(attackPos_, lerpT);
	direction_ = position - pPlayer_->GetPosition();

	pPlayer_->SetPosition(position);
	pPlayer_->GetMesh()->SetRotateY(std::atan2(direction_.x, direction_.z));

	if(lerpT == kMaxT) {
		isEnd_ = true;
	}

}

void PlayerNormalAttackStep2::Exit() {
	PlayerAttackCollider* collider = pPlayer_->GetAttackCollider();
	collider->SetColliderActive(false);
}


bool PlayerNormalAttackStep2::IsEnd() {
	return isEnd_;
}

int PlayerNormalAttackStep2::NextStateIndex() {
	return PlayerStateOrder_Root;
}

void PlayerNormalAttackStep2::Debug() {
	
}


void PlayerNormalAttackStep2::Save() {
	/// jsonにattackPos_を保存
	json root = json::object();

	for(int i = 0; i < attackPos_.size(); i++) {
		root["attackPos"][i]["x"] = attackPos_[i].x;
		root["attackPos"][i]["y"] = attackPos_[i].y;
		root["attackPos"][i]["z"] = attackPos_[i].z;
	}

	/// fileに保存

}

void PlayerNormalAttackStep2::Load() {


}

Vec3 PlayerNormalAttackStep2::Vec3ArrayLerp(const std::array<Vec3, 4>& _array, float _t) {
	int segment = static_cast<int>(_t); // 区間番号 (0, 1, ...)
	if(segment + 1 >= _array.size()) {
		return _array.back();
	}

	float localT = _t - segment;
	return _array[segment] + localT * (_array[segment + 1] - _array[segment]);
}
