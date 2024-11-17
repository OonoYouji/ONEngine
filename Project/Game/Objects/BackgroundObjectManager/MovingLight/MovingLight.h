#pragma once

#include "../BaseBackgroundObject/BaseBackgroundObject.h"

class MovingLight : public BaseBackgroundObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	MovingLight(const Vec3& wPosition);
	~MovingLight();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	float animationTime_;
	class AnimationRenderer* animationRenderer_ = nullptr;

};
