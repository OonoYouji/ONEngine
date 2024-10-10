#pragma once


#include "ComponentManager/AudioSource/AudioSource.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "GameObjectManager/BaseGameObject.h"
#include "Objects/Player/Player.h"


class Ground : public BaseGameObject {
public:

	Ground() { CreateTag(this); }
	~Ground() {}

	void Initialize() override;
	void Update() override;

	void Debug() override;



	
private:
	
};