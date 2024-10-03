#pragma once

#include <GameObjectManager.h>

#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>

#include"Player/Player.h"

class Ground : public BaseGameObject {
public:

	Ground() { CreateTag(this); }
	~Ground() {}

	void Initialize() override;
	void Update() override;

	void Debug() override;

	void PositionPlayerOnSphere(Player* player);

private:
	
	AudioSource* audioSource_ = nullptr;
};