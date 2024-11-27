#pragma once

#include "Manager/BaseScene.h"

enum CLEAR_LAYER {
	CLEAR_LAYER_DEFAULT,
	CLEAR_LAYER_UI,
	CLEAR_LAYER_TRANSITION,
};

class Scene_Clear final : public BaseScene {
public:
	Scene_Clear() {}
	~Scene_Clear() {}

	void Initialize() override;
	void Update() override;
private:
	class SceneTransition* sceneTransition_ = nullptr;
	class SEObj* seObj_ = nullptr;

};