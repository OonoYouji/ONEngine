#pragma once

#include "Scenes/Manager/BaseScene.h"

#include"Objects/Building/BuildingManager.h"

enum TutorialState {
	SCACLEUP,
	SCALEDOWN,
	BUILDINGSPOWN,
	AFEWBUILDINGSPOWN,
	BUINDINGENTRAINMENT,
	BUINDINGSENTRAINMENT,
	DANYATTACK
};

class Scene_Tutorial final : public BaseScene {
public:
	Scene_Tutorial() {}
	~Scene_Tutorial() {}

	void Initialize() override;
	void Update() override;

private:
	TutorialState tutorialState_;
	Player* player_ = nullptr;
	BuildingManager* buildingManager_ = nullptr;
	Tornado* tornado_ = nullptr;
	//パラメータ
	const float kTornadoScaleMax = 2.4f;
	const float kTornadoScaleMin = 1.4f;
};