#pragma once

#include"Game/Scenes/Manager/BaseScene.h"


enum GAME_SCENE_LAYER {
	GAME_SCENE_LAYER_DEFAULT, /// デフォルトのレイヤー
	GAME_SCENE_LAYER_NUMBER,  /// デフォルトのレイヤー
	GAME_SCENE_LAYER_UI,	  /// UI層のレイヤー
	GAME_SCENE_LAYER_TRANSITION,	  /// UI層のレイヤー
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
	class GameManagerObject* gameManager_ = nullptr;
	class SceneTransition* sceneTransition_ = nullptr;
	bool isStartInTransition_ = false;
	bool isEndInTransition_ = false;

	bool isStartOutTransition_ = false;
	int nextScene_;
};