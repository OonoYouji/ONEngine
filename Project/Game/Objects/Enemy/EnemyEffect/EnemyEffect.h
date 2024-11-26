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
	void SetIsActive(bool isActive);
	void SetEffectAnimationRender(const std::string& filePath);
	void SetEffectAnimationTotalTime(float _totalTime);
	void SetEffectAnimationFlags(int _flags,bool _isResetTime);
};