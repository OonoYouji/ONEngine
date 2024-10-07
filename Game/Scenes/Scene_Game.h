#pragma once

#include <BaseScene.h>
#include"Quaternion.h"
#include"Matrix3x3.h"
#include"Vector3.h"

#include "Player/Player.h"
#include"Building/BuildingManager.h"
/// ===================================================
/// ゲームシーン
/// ===================================================
class Scene_Game final : public BaseScene {
public:
	Scene_Game() {}
	~Scene_Game() {}

	Player* p1;
	BuildingManager* buildingManager_;
	void Initialize() override;
	void Update() override;
};