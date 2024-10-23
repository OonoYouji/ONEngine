#pragma once

#include <memory>

#include "Scenes/Manager/BaseScene.h"

#include"Objects/Building/BuildingManager.h"
#include"Objects/Dumy/Dumy.h"
//UI
#include"UI/TutorialUI.h"
#include"UI/GuidanceArrow.h"

#include "ComponentManager/AudioSource/AudioSource.h"


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
	//void CreateUI();
private:
	TutorialState tutorialState_;
	Player* player_ = nullptr;
	Dumy* dumy_ = nullptr;
	BuildingManager* buildingManager_ = nullptr;
	Tornado* tornado_ = nullptr;
	GuidanceArrow* guidanceArrow_ = nullptr;
	//UI
	TutorialScaleUpUI* tutorialScaleUpUI_ = nullptr;
	TutorialScaleDownUI* tutorialScaleDownUI_ = nullptr;
	TutorialEntrainment* tutorialEntrainment_=nullptr;
	TutorialEntrainmentAll* tutorialEntrainmentAll_ = nullptr;
	TutorialBodyBlow* tutorialBodyBlow_ = nullptr;
	bool isCreateUI_;


	std::unique_ptr<AudioSource> bgm_ = nullptr;
};