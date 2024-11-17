#pragma once

#include"Game/Scenes/Manager/BaseScene.h"

enum SCENE_GAME_LAYER {
	SCENE_GAME_LAYER_DEFAULT,
	SCENE_GAME_LAYER_DEFEATED_ENEMY,
	SCENE_GAME_LAYER_UI,
	SCENE_GAME_LAYER_RETICLE,
	SCENE_GAME_LAYER_TRANSITION,
};


enum GAME_SCENE_LAYER {
	GAME_SCENE_LAYER_DEFAULT, /// デフォルトのレイヤー
	GAME_SCENE_LAYER_UI,	  /// UI層のレイヤー
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


	class GameManager*     gameManager_     = nullptr;
	class SceneTransition* sceneTransition_ = nullptr;
	class RailCamera*      railCamera_      = nullptr;

	bool isStarted_ = false;
};