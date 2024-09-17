#include "Player.h"

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>


void Player::Initialize() {
	/*auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("GameScreen");*/
	
	sprite_ = AddComponent<SpriteRenderer>();
	sprite_->SetTexture("uvChecker.png");

	audioSource_ = AddComponent<AudioSource>();
	audioSource_->SetAudioClip("sentaku.wav");

}

void Player::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		audioSource_->PlayAudio();
	}
	
	if(Input::TriggerKey(KeyCode::Alpha2)) {
		sprite_->SetTexture("white2x2.png");
	}
		
	if(Input::TriggerKey(KeyCode::Alpha1)) {
		sprite_->SetTexture("uvChecker.png");
	}

}

