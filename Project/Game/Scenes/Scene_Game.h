#pragma once

#include "Scenes/Manager/BaseScene.h"

/// ===================================================
/// ゲームシーン
/// ===================================================
class Scene_Game final : public BaseScene {
public:
	Scene_Game() {}
	~Scene_Game() {}

	void Initialize() override;
	void Update() override;
};