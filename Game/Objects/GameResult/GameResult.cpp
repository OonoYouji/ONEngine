#define NOMINMAX
#include "GameResult.h"

#include <WorldTime.h>
#include <Easing.h>
#include <AudioSource.h>

#include "Wave/Wave.h"


void GameResult::Initialize() {

	pWave_ = new Wave;
	pWave_->Initialize();
	pWave_->SetAmplitude(1.0f);
	pWave_->SetOffsetY(520.0f);

	/// 1, sosei_beginを流す
	sosei_begin_ = new AudioSource();
	sosei_begin_->SetAudioClip("sosei_begin.wav");
	sosei_begin_->PlayAudio();

	/// 2, 心臓が膨張 → 破裂(パーティクルを出す)


	/// 3, カメラをタイトルのスタート地点に移動させる

	/// 4, 倒した分の敵が落ちてくる

	/// 5, スコアが増えていく(数字)、モニター全体が埋まっていい

	
}

void GameResult::Update() {

	WaveUpdate();

}

void GameResult::BackSpriteDraw() {
}

void GameResult::WaveUpdate() {
	waveAnimationTime_ += WorldTime::DeltaTime();

	waveAnimationSpeed_ = std::sin(waveAnimationTime_) * 10.0f;
	float sinValue = std::sin(waveAnimationSpeed_ * waveAnimationTime_);
	float lerpT = 1.0f - std::min(waveAnimationTime_ / waveMaxAnimationTime_, 1.0f);
	//pWave_->SetAmplitude(Ease::InOut::Bounce(sinValue * lerpT) * 300.0f);

}
