#include "DamageNumRender.h"

#include "FrameManager/Time.h"

#include "ComponentManager/NumberRenderer/NumberRenderer.h"
#include "Scenes/Scene_Game.h"


DamageNumRender::DamageNumRender(uint32_t _score) : score_(_score) {
	CreateTag(this);
}

DamageNumRender::~DamageNumRender() {}

void DamageNumRender::Initialize() {

	lifeTime_ = 5.0f;
	
	pTransform_->scale = Vec3::kOne * 3.0f;

	numberRenderer_ = AddComponent<NumberRenderer>(3);
	numberRenderer_->SetScore(score_);

}

void DamageNumRender::Update() {

	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ < 0.0f) {
		Destory();
	}

}

