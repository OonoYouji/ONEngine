#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>

//std

#include<string>
class Boss;
class BaseBossBehavior{
public:

	BaseBossBehavior(const std::string& name, Boss* boss) :name_(name), pBoss_(boss) {}
	
	virtual ~BaseBossBehavior() {}

	virtual void Update()=0;
protected:
	//振る舞い名
	std::string name_;
	//ボス
	Boss*pBoss_ = nullptr;
	
};