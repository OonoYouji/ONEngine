#pragma once

#include "SceneManager/BaseScene.h"

enum CLEAR_SCENE_LAYER {
	CLEAR_SCENE_LAYER_DEFAULT,
	CLEAR_SCENE_LAYER_UI,
};

class Scene_Clear final : public BaseScene {
public:
	Scene_Clear() {}
	~Scene_Clear() {}

	void Initialize() override;
	void Update() override;
};