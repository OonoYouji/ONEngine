#include "BGMObj.h"

#include "ComponentManager/AudioSource/AudioSource.h"

BGMObj::BGMObj(const std::string& _filePath)
	: filePath_(_filePath) {
	CreateTag(this);
}

BGMObj::~BGMObj() {
	Stop();
}

void BGMObj::Initialize() {
	bgm_ = AddComponent<AudioSource>();
	bgm_->SetAudioClip(filePath_);

	bgm_->volume = 0.01f;
	bgm_->isLoop = true;

	Play();
}

void BGMObj::Update() {

}


void BGMObj::Play() {
	bgm_->PlayAudio();
}

void BGMObj::Stop() {
	bgm_->StopAudioAll();
}

