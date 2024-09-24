#include "Player.h"

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>

#include <ImGuiManager.h>


void Player::Initialize() {
	//auto meshRenderer = AddComponent<MeshRenderer>();
	//meshRenderer->SetModel("TV");
	//meshRenderer->SetMaterial("uvChecker");

	auto sprite = AddComponent<SpriteRenderer>();
	sprite->SetTexture("uvChecker.png");
	
	audioSource_ = AddComponent<AudioSource>();
	audioSource_->SetAudioClip("sentaku.wav");

}

void Player::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		audioSource_->PlayOneShot("sentaku.wav", 1.0f);
	}

	if(Input::TriggerKey(KeyCode::Return)) {
		audioSource_->PlayOneShot("fanfare.wav", 1.0f);
	}

}

void Player::Debug() {
	
}

