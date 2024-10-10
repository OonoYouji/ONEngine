#pragma once

#include "Scenes/Manager/BaseScene.h"

#include "Player/Player.h"
#include"Building/BuildingManager.h"
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
	BuildingManager* buildingManager_;
	Player* player_ = nullptr;
};