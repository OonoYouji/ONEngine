#pragma once

#include "SceneManager/BaseScene.h"

class Scene_Title final : public BaseScene {
public:
	Scene_Title() {}
	~Scene_Title() {}

	void Initialize() override;
	void Update() override;

private:

	class GameSelector*    gameSelector_    = nullptr;
	class SceneTransition* sceneTransition_ = nullptr;
};