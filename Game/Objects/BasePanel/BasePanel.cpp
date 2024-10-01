#include "BasePanel.h"

#include <Input.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
#include <Particle/ParticleSystem.h>
	
#include <ImGuiManager.h>


void BasePanel::Initialize(Vector3 pos) {
	
	audioSource_ = AddComponent<AudioSource>();
	audioSource_->SetAudioClip("sentaku.wav");

	//auto particle = AddComponent<ParticleSystem>(12, "Sphere");

	//SetPositionZ(10.0f);
	auto mesh= AddComponent<MeshRenderer>();
	mesh->SetModel()
	UpdateMatrix();
}

void BasePanel::Update() {



}

void BasePanel::Debug() {
	
}

