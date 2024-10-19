#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>

//std

#include<string>
class Enemy;
class BaseEnemyBehavior{
public:

	BaseEnemyBehavior(const std::string& name, Enemy* enemy) :name_(name), pEnemy_(enemy) {}
	
	virtual ~BaseEnemyBehavior() {}

	virtual void Update()=0;
protected:
	//振る舞い名
	std::string name_;
	//ボス
	Enemy*pEnemy_ = nullptr;
	
};