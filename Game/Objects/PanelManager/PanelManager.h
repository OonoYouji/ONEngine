#pragma once

#include <GameObjectManager.h>

#include <Component/AudioSource/AudioSource.h>
#include <Component/SpriteRenderer/SpriteRenderer.h>

#include"Panel/Panel.h"
class PanelManager : public BaseGameObject {
public:

	PanelManager() { CreateTag(this); }
	~PanelManager() {}

	void Initialize() override;
	void Update() override;

	void Debug() override;

private:
	std::list<Panel>panels_;
	AudioSource* audioSource_ = nullptr;
};