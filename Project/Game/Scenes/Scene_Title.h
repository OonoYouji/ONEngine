#pragma once

#include "Scenes/Manager/BaseScene.h"

enum TITLE_LAYER {
	TITLE_LAYER_DEFAULT,    /// デフォルトのレイヤー
	TITLE_LAYER_UI,         /// UIのレイヤー
	TITLE_LAYER_TRANSITION, /// シーン遷移のレイヤー
};

class Scene_Title final : public BaseScene {
public:
	Scene_Title() {}
	~Scene_Title() {}

	void Initialize() override;
	void Update() override;

private:

	class TitleMenuSelector* menuSelector_ = nullptr;
	class SceneTransition* sceneTransition_ = nullptr;
};