#pragma once

#include "SceneManager/BaseScene.h"

enum TITLE_SCENE_LAYER {
	TITLE_SCENE_LAYER_DEFAULT,
	TITLE_SCENE_LAYER_UI,
};

class Scene_Title final : public BaseScene {
public:
	Scene_Title() {}
	~Scene_Title() {}

	void Initialize() override;
	void Update() override;

};