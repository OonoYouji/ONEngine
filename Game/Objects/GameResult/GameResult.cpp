#include "GameResult.h"

#include <ModelManager.h>
#include <Input.h>

void GameResult::Initialize() {
	paper_ = ModelManager::Load("paper");
	binder_ = ModelManager::Load("binder");

	audioSource_ = new AudioSource();
	audioSource_->SetAudioClip("fanfare.wav");
	audioSource_->PlayAudio();
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
