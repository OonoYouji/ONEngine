#pragma once

#include "GameObjectManager/GameObjectManager.h"


enum TRANSITION_ORDER {
	TRANSTION_FADE_IN,
	TRANSTION_FADE_OUT,
};


class SceneTransition : public BaseGameObject {
public:

	SceneTransition(int _transitionOrder);
	~SceneTransition();

	void Initialize() override;
	void Update()     override;

	void SetTransitionOrder(int _transitionOrder);

	bool GetIsFinished() const;

private:

	void UpdateFadeIn();
	void UpdateFadeOut();

private:
	class SpriteRenderer* spriteRenderer_ = nullptr;

	int transitionOrder_ = 0;

	/// animation params
	float animationTime_    = 0.0f;
	float maxAnimationTime_ = 1.0f;

};
