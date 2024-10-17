#pragma once

#include "Scenes/Manager/BaseScene.h"

#include"Objects/Building/BuildingManager.h"

class Scene_Tutorial final : public BaseScene {
public:
	Scene_Tutorial() {}
	~Scene_Tutorial() {}

	void Initialize() override;
	void Update() override;

private:
	BuildingManager* buildingManager_ = nullptr;
	Tornado* tornado_ = nullptr;
};