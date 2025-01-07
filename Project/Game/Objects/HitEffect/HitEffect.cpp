#include "HitEffect.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "FrameManager/Time.h"

/// user
#include "Objects/Camera/GameCamera.h"


HitEffect::HitEffect(GameCamera* _camera)
	: pCamera_(_camera) {
	CreateTag(this);
}

HitEffect::~HitEffect() {}

void HitEffect::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("HitEffect");
	meshRenderer_->SetMaterial("HitEffect-sheet.png");
	meshRenderer_->SetColor({ 1,1,1,1.0f });
	meshRenderer_->SetIsLighting(false);
	meshRenderer_->SetUVScale({ 1.0f / 6.0f, 1.0f});

	time_          = 0.0f;
	frameDuration_ = 0.01f;

}

void HitEffect::Update() {

	pTransform_->rotate = pCamera_->GetRotate();
	pTransform_->scale += Vec3::kOne * 0.5f;

	time_ += Time::DeltaTime();
	if(time_ > frameDuration_) {
		time_ = 0.0f;
		frameIndex_++;
		meshRenderer_->SetUVPosition({ 1.0f / 6.0f * frameIndex_, 0.0f });
	}

	if(frameIndex_ >= 6) {
		Destory();
		//frameIndex_ = 0;
	}

}

