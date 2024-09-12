#define NOMINMAX
#include "GameResult.h"

#include <CameraManager.h>

#include <WorldTime.h>
#include <Easing.h>
#include <AudioSource.h>


#include "Heart/Heart.h"
#include "Hand/Hand.h"
#include "Wave/Wave.h"
#include "EnemyComboManager/EnemyComboManager.h"


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


	/// 4, 倒した分の敵が落ちてくる
	killedEnemiesCount_ = EnemyComboManager::GetKilledEnemiesCount();


	/// 5, スコアが増えていく(数字)、モニター全体が埋まっていい


}

void GameResult::Update() {

	switch(effectPhase_) {
	case HEART_BERAK:				/// 2, 心臓が膨張→破裂(パーティクルを出す) 

		HeartBreak();
		break;
	case CAMERA_MOVING:				/// 3, カメラをタイトルのスタート地点に移動させる

		CameraMoving();
		break;
	case KILLED_ENEMIES_DROPING:	/// 4, 倒した分の敵が落ちてくる


		break;
	case SCORE_CALCULATION:			/// 5, スコアが増えていく(数字)、モニター全体が埋まっていい


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
			//heart_->Destory();
			//heart_ = nullptr;
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
	if(cameraMovingTime_ / cameraMovingMaxTime_ == 1.0f) {
		WaitTime(KILLED_ENEMIES_DROPING, 0.2F);
	}

}

void GameResult::WaitTime(EFFECT_PAHSE nextPahse, float waitTime) {
	effectPhase_ = PAHSE_WAIT;
	waitTime_ = waitTime;
	nextPahse_ = nextPahse;
}

