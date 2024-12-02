#pragma once

#include <memory>
#include "GraphicManager/PostEffect/Grayscale/Grayscale.h"

#include "SceneManager/BaseScene.h"


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

};