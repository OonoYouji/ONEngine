#pragma once

#include <GameObjectManager.h>

#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>

class Player : public BaseGameObject {
public:

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update() override;

	void Debug() override;

private:
	AudioSource* audioSource_ = nullptr;
};