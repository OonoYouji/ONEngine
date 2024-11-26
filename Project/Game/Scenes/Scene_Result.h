#pragma once

#include "Scenes/Manager/BaseScene.h"


enum RESULT_LAYER {
	RESULT_LAYER_DEFAULT,
	RESULT_LAYER_UI,
	RESULT_LAYER_TRANSITION,
};


class Scene_Result final : public BaseScene {
public:
	Scene_Result() {}
	~Scene_Result() {}

	void Initialize() override;
	void Update() override;

private:
	class SceneTransition* sceneTransition_ = nullptr;

};