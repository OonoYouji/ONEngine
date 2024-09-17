#include "Player.h"

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>


void Player::Initialize() {
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("GameScreen");
	meshRenderer->SetMaterial("white2x2");
	
	audioSource_ = AddComponent<AudioSource>();
	audioSource_->SetAudioClip("sentaku.wav");

	fanfare_ = AddComponent<AudioSource>();
	fanfare_->SetAudioClip("fanfare.wav");

}

void Player::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		audioSource_->PlayAudio();
	}
	
	if(Input::TriggerKey(KeyCode::Return)) {
		fanfare_->PlayAudio();
	}
	
}

