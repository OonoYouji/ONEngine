#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>
//std
#include<optional>
#include"Objects/BossBehavior/BaseBossBehavior.h"

class Boss : public BaseGameObject {
public:

	Boss() { CreateTag(this); }
	~Boss() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	////状態変更
	//void ChangeState(std::unique_ptr<BaseBossBehavior>behavior);
	
private:
	
private:
	////状態
	//std::unique_ptr<BaseBossBehavior>behavior_;
	//ピボット
	Transform pivot_;
	
};