#include "Player.h"

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include <Particle/ParticleSystem.h>
	
#include <ImGuiManager.h>


void Player::Initialize() {
	//auto meshRenderer = AddComponent<MeshRenderer>();
	//meshRenderer->SetModel("Sphere");
	//meshRenderer->SetMaterial("uvChecker");

	auto sprite = AddComponent<SpriteRenderer>();
	sprite->SetTexture("uvChecker.png");
	
	audioSource_ = AddComponent<AudioSource>();
	audioSource_->SetAudioClip("sentaku.wav");

	auto particle = AddComponent<ParticleSystem>(12);

	SetPositionZ(10.0f);
	UpdateMatrix();
}

void Player::Update() {

	Vec3 velocity{
		static_cast<float>(Input::PressKey(KeyCode::d) - Input::PressKey(KeyCode::a)),
		static_cast<float>(Input::PressKey(KeyCode::w) - Input::PressKey(KeyCode::s)),
		0.0f
	};

	SetPosition(GetPosition() + velocity * 0.25f);

}

void Player::Debug() {
	
}

