#pragma once

#include <memory>

#include "Scenes/Manager/BaseScene.h"

#include "ComponentManager/AudioSource/AudioSource.h"

class Scene_Result final : public BaseScene {
public:
	Scene_Result() {}
	~Scene_Result() {}

	void Initialize() override;
	void Update() override;

private:
	class GameResult* gameResult_ = nullptr;

	std::unique_ptr<AudioSource> bgm_;
};