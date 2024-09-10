#include "GameResult.h"

#include <ModelManager.h>
#include <Input.h>

void GameResult::Initialize() {
	drawLayerId = 1;
	paper_ = ModelManager::Load("paper");
	binder_ = ModelManager::Load("binder");

	SetPosition({ 7.55f, 0.5f, 0.5f });
	SetRotate({ -1.15f, 0.4f, 0.0f });
	UpdateMatrix();

	audioSource_ = new AudioSource();
	audioSource_->SetAudioClip("fanfare.wav");
}

void GameResult::Update() {
	if(Input::TriggerKey(KeyCode::Space)) {
		audioSource_->StopAudioAll();
	}

	if(Input::TriggerKey(KeyCode::P)) {
		audioSource_->PlayAudio();
	}

	if(Input::TriggerKey(KeyCode::L)) {
		audioSource_->isLoop = true;
	}
}

void GameResult::Draw() {
	paper_->Draw(&transform_);
	binder_->Draw(&transform_);
}
