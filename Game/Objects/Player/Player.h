#pragma once

#include <GameObjectManager.h>

#include <AudioSource.h>

class Player : public BaseGameObject {
public:

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update() override;

private:

	AudioSource* audioSource_ = nullptr;

};