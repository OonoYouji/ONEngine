#pragma once

#include "GameObjectManager/BaseGameObject.h"

class GameClearEffect : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameClearEffect();
	~GameClearEffect();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class ClearEffectSprite* clearEffectSprite_ = nullptr;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	bool IsEnd() const;

};



/// ===================================================
/// クリア演出のスプライト
/// ===================================================
class ClearEffectSprite : public BaseGameObject {
public:

	ClearEffectSprite();
	~ClearEffectSprite();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


private:
	
	const Vec2&&  kTextureSize_ = { 2048.0f, 720.0f };
	const float&& kOffsetX_ = 1024.0f;

	class SpriteRenderer* spriteRenderer_ = nullptr;
	Vec2 uvPosition_     = { 0.0f, 0.0f };
	Vec2 scrollVelocity_ = { 10.0f, 0.0f };

	float currentTime_;
	float durationTime_;
	float colorMaxLerpTime_;

	bool isEnd_ = false;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	bool IsEnd() const { return isEnd_; }

};



/// ===================================================
/// クリア演出のパーティクル
/// ===================================================
class ClearEffectParticle : public BaseGameObject {
public:

	ClearEffectParticle();
	~ClearEffectParticle();

	void Initialize() override;
	void Update()     override;

private:
};