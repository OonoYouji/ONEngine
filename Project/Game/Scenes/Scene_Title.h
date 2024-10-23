#pragma once

/// std
#include <memory>

/// components
#include "ComponentManager/AudioSource/AudioSource.h"

/// base class
#include "Scenes/Manager/BaseScene.h"


class Scene_Title final : public BaseScene {
public:
	Scene_Title() {}
	~Scene_Title() {}

	void Initialize() override;
	void Update()     override;
private:
	class SceneTransition* sceneTransition_ = nullptr;
	

private:

	std::unique_ptr<AudioSource> audioSource_ = nullptr;
};