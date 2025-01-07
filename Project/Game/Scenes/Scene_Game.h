#pragma once

#include <array>

#include "SceneManager/BaseScene.h"

enum GAME_SCENE_LAYER {
	GAME_SCENE_LAYER_DEFAULT, /// デフォルトのレイヤー
	GAME_SCENE_LAYER_UI,	  /// UI層のレイヤー
	GAME_SCENE_LAYER_TARGET_SPRITE,	  /// UI層のレイヤー
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
	std::array<class PointLight*, 5> pointLightArray_;
	class EnemyManager* enemyManager_ = nullptr;
};