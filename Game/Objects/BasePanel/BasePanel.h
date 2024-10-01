#pragma once

#include <GameObjectManager.h>

#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>

class BasePanel : public BaseGameObject {
public:

	BasePanel() { CreateTag(this); }
	~BasePanel() {}

	virtual	void Initialize(Vector3 pos);
	virtual	void Update() override;

	virtual void Debug() override;

private:
	
	AudioSource* audioSource_ = nullptr;
};