#pragma once

#include <GameObjectManager.h>

#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>
//std
#include<optional>

class Boss : public BaseGameObject {
public:

	Boss() { CreateTag(this); }
	~Boss() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	
private:
	
private:
	//ピボット
	Transform pivot_;
	
};