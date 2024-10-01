#pragma once
#include "BasePanel/BasePanel.h"
//#include <GameObjectManager.h>
//
//#include <Component/AudioSource/AudioSource.h>
//#include <Component/SpriteRenderer/SpriteRenderer.h>

class Panel : public BasePanel {
public:

	Panel() { CreateTag(this); }
	~Panel() {}

	void Initialize() override;
	void Update() override;

	void Debug() override;

private:

	AudioSource* audioSource_ = nullptr;
};