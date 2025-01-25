#include "GameClearEffect.h"

/// std
#include <numbers>
#include <algorithm>

/// externals
#include <imgui.h>

/// engine
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"
#include "FrameManager/Time.h"
#include "Math/Easing.h"

/// game
#include "Scenes/Scene_Game.h"


GameClearEffect::GameClearEffect() {
	CreateTag(this);
}

GameClearEffect::~GameClearEffect() {}


void GameClearEffect::Initialize() {

	clearEffectSprite_ = new ClearEffectSprite();
	clearEffectSprite_->Initialize();
	clearEffectSprite_->SetParent(pTransform_);
	
}

void GameClearEffect::Update() {

}

bool GameClearEffect::IsEnd() const {
	return clearEffectSprite_->IsEnd();
}

/// ===================================================
/// クリア演出のスプライト
/// ===================================================

ClearEffectSprite::ClearEffectSprite() {
	CreateTag(this);
}
ClearEffectSprite::~ClearEffectSprite() {}


void ClearEffectSprite::Initialize() {

	/// 表示するLayerの設定
	drawLayerId = GAME_SCENE_LAYER_CLEAR_EFFECT;

	/// SpriteRendererの初期化
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("GameClear.png");

	pTransform_->scale = { 10.0f, 6.0f, 1.0f };

	scrollVelocity_.x = 250.0f;
	uvPosition_.y = 350.0f;

	spriteRenderer_->SetUVRotate(std::numbers::pi_v<float> * 0.25f * -1.0f);
	spriteRenderer_->SetColor({ 1.0f, 1.0f, 1.0f, 0.5f });

	currentTime_ = 0.0f;
	durationTime_ = 1.0f;

	colorMaxLerpTime_ = 1.0f;
}

void ClearEffectSprite::Update() {

	/// uvのスクロール
	currentTime_ += Time::DeltaTime();
	float&& lerpT = std::fmod(currentTime_, durationTime_) / durationTime_;
	uvPosition_   = Vec2::Lerp(
		{ 0.0f + kOffsetX_, 360.0f },
		{ kTextureSize_.x + kOffsetX_, 360.0f }, 
		Ease::Out::Expo(lerpT)
	);


	/// alphaを線形補完
	float   colorLerpT = std::clamp(currentTime_ / colorMaxLerpTime_, 0.0f, 1.0f);
	float&& alpha      = std::lerp(0.0f, 1.0f, Ease::In::Expo(colorLerpT));
	spriteRenderer_->SetColor(Vec4(Vec3::kOne, colorLerpT));


	spriteRenderer_->SetUVPosition(uvPosition_);


	/// 演出の終了条件
	if (currentTime_ > 2.0f) {
		isEnd_ = true;
	}


}

void ClearEffectSprite::Debug() {

	ImGui::DragFloat2("scroll velocity", &scrollVelocity_.x, 0.1f);
	ImGui::DragFloat2("uv position", &uvPosition_.x, 0.1f);

	ImGui::Spacing();

	ImGui::DragFloat("current time", &currentTime_, 0.1f);
	ImGui::DragFloat("duration time", &durationTime_, 0.1f);

}


/// ===================================================
/// クリア演出のパーティクル
/// ===================================================

ClearEffectParticle::ClearEffectParticle() {
	CreateTag(this);
}
ClearEffectParticle::~ClearEffectParticle() {}

void ClearEffectParticle::Initialize() {

}

void ClearEffectParticle::Update() {

}
