#define NOMINMAX
#include "GameResult.h"

#include <cmath>

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


	/// 4, 倒した分の敵が落ちてくる
	killedEnemiesCount_ = EnemyComboManager::GetKilledEnemiesCount();
	killedEnemiesCount_ = 100u;
	largeEnemySpriteNum_ = killedEnemiesCount_ / 20u + 1u;

	dropDatas_.resize(std::min(largeEnemySpriteNum_, 22u));
	if(largeEnemySpriteNum_ != dropDatas_.size()) {
		largeEnemySpriteNum_ = static_cast<uint32_t>(dropDatas_.size());
	}

	for(uint32_t i = 0u; i < largeEnemySpriteNum_; ++i) {
		auto& dropData = dropDatas_[i];
		dropData.sprite.reset(new Sprite);
		dropData.sprite->Initialize("largeEnemy", "largeEnemy.png");
		dropData.sprite->SetPos({ 640.0f, -20.0f, 0.0f });
		dropData.sprite->SetSize({ 640.0f, 45.0f });

		dropData.isStart = false;
		dropData.time = 0.0f;

		dropData.endPosition = Vec3(
			640.0f + Random::Float(-50.0f, 50.0f),
			520.0f - (i * 45.f * 0.5f),
			0.0f
		);

		dropData.startPosition = Vec3(
			dropData.endPosition.x,
			-20.0f,
			0.0f
		);

	}

	dropDatas_.front().isStart = true;


	/// 5, スコアが増えていく(数字)、モニター全体が埋まっていい

	totalScore_ = ScoreManager::GetScoreCount();
	totalScore_ = 999999;

	uint32_t digit = static_cast<uint32_t>(std::log10(totalScore_)) + 1u;

	texDatas_.resize(10);
	for(uint8_t i = 0u; i < 10u; ++i) {
		auto& texData = texDatas_[i];
		texData.texName = std::to_string(i);
		texData.filePath = texData.texName + std::string(".png");
	}

	digitNumbers_.resize(digit);
	for(uint32_t i = 0u; i < digit; ++i) {
		auto& number = digitNumbers_[i];
		number.reset(new Sprite);
		number->Initialize(texDatas_[i].texName, texDatas_[i].filePath);
		number->SetSize({ 50.0f, 60.0f });

		number->SetPos(Vec3(
			640.0f + ((float(digit) * 0.5f - i) * 100.0f) - 50.0f,
			320.0f,
			0.0f
		));
	}

}

void GameResult::Update() {

	switch(effectPhase_) {
	case HEART_BERAK:				/// 2, 心臓が膨張→破裂(パーティクルを出す) 

		HeartBreak();
		break;
	case CAMERA_MOVING:				/// 3, カメラをタイトルのスタート地点に移動させる

		CameraMoving();
		break;
	case KILLED_ENEMIES_LEAVE:	/// 4, 倒した分の敵が落ちてくる

		KilledEnemiesLeave();
		break;
	case KILLED_ENEMIES_DROPING:	/// 4, 倒した分の敵が落ちてくる

		KilledEnemiesDropping();
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

	if(isDrawScore_) {
		for(auto& number : digitNumbers_) {
			number->Draw();
		}
	}


	if(effectPhase_ == KILLED_ENEMIES_LEAVE
	   || effectPhase_ == KILLED_ENEMIES_DROPING
	   || effectPhase_ == PAHSE_WAIT) {

		for(auto& dropData : dropDatas_) {
			dropData.sprite->Draw();
		}
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
		WaitTime(KILLED_ENEMIES_LEAVE, 0.2F);
	}

}

void GameResult::KilledEnemiesLeave() {

	for(auto itr = dropDatas_.begin(); itr != dropDatas_.end(); ++itr) {

		auto& dropData = (*itr);

		if(!dropData.isStart) {
			continue;
		}

		dropData.time += WorldTime::DeltaTime();
		float lerpT = std::min(dropData.time / droppingMaxAnimationTime_, 1.0f);

		dropData.sprite->SetPos(Vec3::Lerp(
			dropData.startPosition, dropData.endPosition,
			lerpT
		));

		if(lerpT >= 0.2f) {
			auto next = std::next(itr);
			if(next != dropDatas_.end()) {
				next->isStart = true;
			}
		}

		if(lerpT == 1.0f) {
			dropData.sprite->SetTexture("largeEnemyStamp", "largeEnemyStamp.png");
		}

	}

	///	次の計算に行くための条件
	if(dropDatas_.back().time / droppingMaxAnimationTime_ >= 1.0f) {
		WaitTime(KILLED_ENEMIES_DROPING, 1.0f);

		for(auto& dropData : dropDatas_) {
			dropData.startPosition = dropData.endPosition;
			dropData.endPosition.y += 720.0f;
			dropData.time = 0.0f;
		}
	}
}



void GameResult::KilledEnemiesDropping() {

	isDrawScore_ = true;

	float alpha = std::max(waveAlphaLerpTime_ / 0.5f, 0.0f);
	waveAlphaLerpTime_ -= WorldTime::DeltaTime();

	pWave_->SetAlpha(alpha);

	if(alpha == 0.0f) {

		for(auto& dropData : dropDatas_) {

			dropData.time += WorldTime::DeltaTime();
			float lerpT = std::min(dropData.time / droppingMaxAnimationTime_, 1.0f);

			dropData.sprite->SetPos(Vec3::Lerp(
				dropData.startPosition, dropData.endPosition,
				lerpT
			));
		}
	}

	/// 落下中にスコアを計算している風にする
	for(auto& number : digitNumbers_) {
		int index = Random::Int(0, 9);
		number->SetTexture(
			texDatas_[index].texName,
			texDatas_[index].filePath
		);
	}


	if(dropDatas_.back().time / droppingMaxAnimationTime_ >= 1.0f) {
		WaitTime(EFFECT_END, 0.1f);


		/// 表示する値をしっかり計算する
		std::vector<int> digits;
		int num = totalScore_;

		if(num == 0) {
			digits.push_back(0);
		} else {
			while(num > 0) {
				digits.push_back(num % 10);
				num /= 10;
			}
		}

		//std::reverse(digits.begin(), digits.end());

		for(uint32_t i = 0u; i < digitNumbers_.size(); ++i) {
			digitNumbers_[i]->SetTexture(
				texDatas_[digits[i]].texName,
				texDatas_[digits[i]].filePath
			);
		}

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

