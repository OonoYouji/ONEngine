#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
//obj
#include"Objects/Boss/Boss.h"
//std
#include<optional>
#include<string>
#include<memory>
class Player;
class BasePlayerBehavior {
public:

	BasePlayerBehavior(const std::string& name, Player* boss) :name_(name), pPlayer_(boss) {}

	virtual ~BasePlayerBehavior() {}

	virtual void Update() = 0;
protected:
	//振る舞い名
	std::string name_;
	//プレイや
	Player* pPlayer_ = nullptr;
};