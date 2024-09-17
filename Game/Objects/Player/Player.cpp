#include "Player.h"

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>


void Player::Initialize() {
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("GameScreen");

	audioSource_ = AddComponent<AudioSource>();
	audioSource_->SetAudioClip("sentaku.wav");
}

void Player::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		audioSource_->PlayAudio();
	}

}

