#pragma once

#include <string>

#include "GameObjectManager/BaseGameObject.h"

class AnimationRenderer;

class EnemyEffect
	:public BaseGameObject{
public:
	EnemyEffect();
	~EnemyEffect();

	void Initialize()override;
	void Update()override;

private:
	AnimationRenderer* effect_;
public:
	AnimationRenderer* GetAnimationRenderer()const;

	void SetIsActive(bool isActive);
	bool GetIsActiveAnimation()const;
	void SetEffectAnimationRender(const std::string& filePath);
	void SetEffectAnimationTotalTime(float _totalTime);
	void ResetAnimationTotal();
	void SetEffectAnimationFlags(int _flags,bool _isResetTime = true);
};