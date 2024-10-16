#pragma once

#include"Game/Scenes/Manager/BaseScene.h"


#include "Objects/Player/Player.h"
#include"Objects/Building/BuildingManager.h"
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
	Tornado* tornado_ = nullptr;
};