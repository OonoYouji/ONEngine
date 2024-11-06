#pragma once

#include"Game/Scenes/Manager/BaseScene.h"

enum SCENE_GAME_LAYER {
	SCENE_GAME_LAYER_DEFAULT,
	SCENE_GAME_LAYER_DEFEATED_ENEMY,
	SCENE_GAME_LAYER_UI,
};


/// ===================================================
/// ゲームシーン
/// ===================================================
class Scene_Game final : public BaseScene {
public:

	 Scene_Game() {}
	~Scene_Game() {}

	void Initialize() override;
	void Update()     override;

private:
};