#define NOMINMAX
#include "GameResult.h"

#include <cmath>

#include <ImGuiManager.h>
#include <SceneManager.h>
#include <CameraManager.h>

#include <AudioSource.h>
#include <WorldTime.h>
#include <Easing.h>
#include <Random/Random.h>
#include <Input.h>

#include "Heart/Heart.h"
#include "Hand/Hand.h"
#include "Wave/Wave.h"
#include "EnemyComboManager/EnemyComboManager.h"
#include "ScoreManager/ScoreManager.h"


void GameResult::Initialize() {

	/// 音はすぐに流すので2からスタート
	effectPhase_ = HEART_BERAK;


	/// 波
	pWave_ = new Wave;
	pWave_->Initialize();
	pWave_->SetAmplitude(1.0f);
	pWave_->SetOffsetY(520.0f);
	pWave_->SetAmplitude(0.0f);


	/// 1, sosei_beginを流す
	sosei_begin_ = new AudioSource();
	sosei_begin_->SetAudioClip("sosei_begin.wav");
	sosei_begin_->PlayAudio();


	/// 2, 心臓が膨張 → 破裂(パーティクルを出す)
	/// 心臓
	heart_ = new Heart;
	heart_->Initialize();
	heart_->SetPosition({ -3.8f, -0.8f, -4.1f });
	heart_->SetRotate({ 0.0f, -1.0f, 0.45f });
	heart_->SetScale(Vec3::kOne * 0.7f);
	heart_->UpdateMatrix();

	/// 心臓の下部を探索
	for(auto& child : heart_->GetChilds()) {
		HeartBottom* bottom = dynamic_cast<HeartBottom*>(child);
		if(bottom) {
			heartBottom_ = bottom;
			break;
		}
	}

	/// 手
	hand_ = new Hand;
	hand_->Initialize();
	hand_->SetPosition({ -4.05f, -0.95f, -3.9f });
	hand_->SetRotate({ 0.0f, -0.5f, 0.0f });
	hand_->SetScale(Vec3::kOne * 0.5f);
	hand_->UpdateMatrix();


	heartBreakEffect_ = new Effect();
	heartBreakEffect_->SetGrainMode(0);
	heartBreakEffect_->Initialize();
	heartBreakEffect_->SetOverType(0);
	heartBreakEffect_->OverTimeSetting(8, 0); /// frame, 出る個数
	heartBreakEffect_->SetVariavles({ 0.0f,-0.025f,0.0f }, 0.5f, { 0.4f,0.4f,0.4f }, 30, true, true, 0.2f, 0.3f);
	heartBreakEffect_->ShapeType(2);
	heartBreakEffect_->SetPos(heart_->GetPosition());


	/// 3, カメラをタイトルのスタート地点に移動させる
	cameraMovingTime_ = 0.0f;
	cameraMovingMaxTime_ = 1.0f;


	/// 4, RIPと今日の日付を表示

	rip_.reset(new Sprite);
	rip_->Initialize("RIP", "RIP.png");

	ripPosition_ = { 640.0f, 300.0f, 0.0f };
	rip_->SetPos(ripPosition_);

	ripSize_ = Vec2(270.0f, 220.0f);
	rip_->SetSize(ripSize_);

	ripAnimationTime_ = 0.0f;
	ripMaxAnimationTime_ = 1.0f;

}

void GameResult::Update() {

	switch(effectPhase_) {
	case HEART_BERAK:				/// 2, 心臓が膨張→破裂(パーティクルを出す) 

		HeartBreak();
		break;
	case CAMERA_MOVING:				/// 3, カメラをタイトルのスタート地点に移動させる

		CameraMoving();
		break;
	case RIP_DRAW:

		RIP();
		break;
	case EFFECT_END:

		EffectEndUpdate();
		break;
	case PAHSE_WAIT:
		waitTime_ -= WorldTime::DeltaTime();
		if(waitTime_ <= 0.0f) {
			effectPhase_ = nextPahse_;
		}
		break;
	}


}

void GameResult::BackSpriteDraw() {}

void GameResult::FrontSpriteDraw() {

	if(ripIsDraw_) {
		rip_->Draw();
	}


}

void GameResult::Debug() {
	if(ImGui::TreeNodeEx("rip", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("position", &ripPosition_.x, 1.0f);
		ImGui::ColorEdit4("color", &ripColor_.x);
		ImGui::DragFloat2("size", &ripSize_.x);

		ImGui::DragFloat("current time", &ripAnimationTime_, 0.1f);
		ImGui::DragFloat("max time", &ripMaxAnimationTime_, 0.1f);


		ImGui::TreePop();
	}
}

void GameResult::HeartBreak() {

	const float kMaxTime = 1.0f;
	const float kMaxScale = 5.0f;

	float lerpT = std::min(heartBreakAnimationTime_ / kMaxTime, 1.0f);

	if(heartBottom_) {
		heartBottom_->SetScale(Vec3::kOne + Vec3::kOne * (kMaxScale * Ease::In::Expo(lerpT)));
		heartBreakAnimationTime_ += WorldTime::DeltaTime();
	}


	/// 次のフェーズに行く
	if(lerpT == 1.0f) {

		heartBreakEffect_->OverTimeSetting(1, 8); /// frame, 出る個数
		heartBreakEffect_->EffectStart();

		isBreaked_ = true;

		if(heart_) {
			heart_->isDrawActive = false;
			for(auto& child : heart_->GetChilds()) {
				child->isDrawActive = false;
			}
		}
	}


	if(isBreaked_) {
		--heartBreakEffectRemainingFrame_;	/// デクリメント
		if(heartBreakEffectRemainingFrame_ == 0u) {
			heartBreakEffect_->EffectStop();
			WaitTime(CAMERA_MOVING, 1.0f);
		}
	}

}

void GameResult::CameraMoving() {

	if(cameraMovingTime_ == 0.0f) {
		BaseCamera* camera =
			CameraManager::GetInstance()->GetCamera("GameCamera");
		camera->SetMove(
			{ { 0.0f, 0.2f, -15.0f }, { 0.0f, -0.12f, 0.0f } },
			{ { 0.0f, 0.3f, -7.6f }, { 0.0f, 0.0f, 0.0f } },
			cameraMovingMaxTime_
		);
	}

	cameraMovingTime_ += WorldTime::DeltaTime();

	/// 移動の終了
	if(cameraMovingTime_ / cameraMovingMaxTime_ >= 1.0f) {
		WaitTime(RIP_DRAW, 0.2F);
	}

}

void GameResult::RIP() {

	ripIsDraw_ = true;

	ripAnimationTime_ += WorldTime::DeltaTime();
	float lerpT = std::min(ripAnimationTime_ / ripMaxAnimationTime_, 1.0f);

	/// 上から下に落とす
	ripPosition_.y = std::lerp(
		-100.0f, 300.0f,
		lerpT
	);

	ripColor_ = Vec4::Lerp(
		{ 0,0,0,0 }, { 1,1,1,1 },
		lerpT
	);

	rip_->SetColor(ripColor_);
	rip_->SetPos(ripPosition_);
	rip_->SetSize(ripSize_);

	/// 次のフェーズに行く
	if(lerpT == 1.0f) {
		//WaitTime(EFFECT_END, 0.1f);
	}

}


void GameResult::EffectEndUpdate() {

	bool inputTriggered = false;
	inputTriggered |= Input::TriggerKey(KeyCode::Enter);
	inputTriggered |= Input::TriggerKey(KeyCode::Space);
	inputTriggered |= Input::TriggerPadButton(PadCode::A);

	if(inputTriggered) {
		SceneManager::GetInstance()->SetNextScene(TITLE);
	}
}



void GameResult::WaitTime(EFFECT_PAHSE nextPahse, float waitTime) {
	effectPhase_ = PAHSE_WAIT;
	waitTime_ = waitTime;
	nextPahse_ = nextPahse;
}

