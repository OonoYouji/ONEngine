#define NOMINMAX
#include "GameResult.h"

#include <WorldTime.h>
#include <Easing.h>

#include "Wave/Wave.h"

void GameResult::Initialize() {

	pWave_ = new Wave;
	pWave_->Initialize();
	pWave_->SetAmplitude(1.0f);
	pWave_->SetOffsetY(520.0f);
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
