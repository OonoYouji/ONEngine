#pragma once

#include <GameObjectManager.h>

class Wave final : public BaseGameObject {
public:
	Wave() { CreateTag(this); }
	~Wave() {  }

	void Initialize() override;
	void Update() override;
	void FrontSpriteDraw() override;

	void Debug() override;

private:
	std::vector<float> waveHeights_;

	float amplitude_ = 1.0f;
	float frequency_ = 1.0f;
	float addlambda_ = 1.0f;
	float offsetY_ = 1.0f;

	float animationTime_ = 0.0f;
	float maxAnimationTime_ = 1.0f;

};