#pragma once

#include "GameObjectManager/GameObjectManager.h"
#include <ComponentManager/Transform/Transform.h>
#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
#include<optional>
//obj
#include"Objects/Tornado/Tornado.h"
#include"Objects/Player/Player.h"


class Dumy : public BaseGameObject {
public:

	Dumy() { CreateTag(this); }
	~Dumy() {}

	virtual	void Initialize()override;
	virtual	void Update() override;
	virtual void Debug() override;
	

	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision)override;
	//getter
	bool GetIsBreak()const { return isBreak_; }

private:
	bool isBreak_;
	uint32_t hp_;
	const uint32_t hpMax_=10;
	Transform pivot_;
};