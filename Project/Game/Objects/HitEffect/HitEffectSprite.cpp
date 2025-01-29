#include "HitEffectSprite.h"

/// std
#include <numbers>

/// engine
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"
#include "Objects/Camera/Manager/CameraManager.h"
#include "FrameManager/Time.h"
#include "Math/Random.h"
#include "Math/Easing.h"

/// user
#include "Scenes/Scene_Game.h"


HitEffectSprite::HitEffectSprite() {
	CreateTag(this);
}

HitEffectSprite::~HitEffectSprite() {}

void HitEffectSprite::Initialize() {

	drawLayerId = GAME_SCENE_LAYER_SPRITE_EFFECT;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("HitEffectLine.png");
	
	color_ = { 1.0f, 1.0f, 1.0f, 0.1f };
	spriteRenderer_->SetColor(color_);


	pTransform_->scale.x = 40.0f;
	pTransform_->scale.y = Random::Float(1.0f, 2.0f);

	rotateZ_ = Random::Float(0.0f, std::numbers::pi_v<float>);


	/// life time param setting
	maxLifeTime_ = 0.2f;
	lifeTime_    = maxLifeTime_;

}

void HitEffectSprite::Update() {

	{	/// ビルボード計算
		BaseCamera* mainCamera = CameraManager::GetInstance()->GetMainCamera();
		pTransform_->rotate = mainCamera->GetRotate();
		pTransform_->rotate.z += rotateZ_;
	}


	lifeTime_ -= Time::DeltaTime();

	float lerpT = 1.0f - (lifeTime_ / maxLifeTime_);
	lerpT = std::clamp(lerpT, 0.0f, 1.0f);


	/// alphaの線形補完
	color_.w = std::lerp(0.2f, 1.0f, Ease::Out::Quint(lerpT));
	spriteRenderer_->SetColor(color_);

	/// scaleの線形補完
	pTransform_->scale.x = std::lerp(10.0f, 20.0f, Ease::Out::Expo(lerpT));
	pTransform_->scale.y = std::lerp(2.0f, 0.2f, lerpT);


	if (lifeTime_ < 0.0f) {
		Destory();
	}
}

