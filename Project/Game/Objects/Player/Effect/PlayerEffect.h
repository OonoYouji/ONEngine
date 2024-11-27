#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerEffect : public BaseGameObject {
	friend class Player;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerEffect();
	~PlayerEffect();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class AnimationRenderer* animationRenderer_ = nullptr;
	float totalTime_ = 0.0f;

public:
	void ActiveAnimation();
	void SetTotalTime(float _totalTime);


	void OneShotAnimation(const std::string& _filePath, float _totalTime);
};
