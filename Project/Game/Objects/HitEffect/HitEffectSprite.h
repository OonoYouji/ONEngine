#pragma once

#include "GameObjectManager/BaseGameObject.h"

class HitEffectSprite : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	HitEffectSprite();
	~HitEffectSprite();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

	float rotateZ_;

	float maxLifeTime_;
	float lifeTime_;

	Vec4 color_;

};
