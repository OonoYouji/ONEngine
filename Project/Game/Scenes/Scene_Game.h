#pragma once

#include"Game/Scenes/Manager/BaseScene.h"


#include "Objects/Player/Player.h"
#include "Objects/Boss/Boss.h"
#include "Objects/Boss/BossBulletLump.h"
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

	static int sGameResult_;
private:
	BuildingManager* buildingManager_=nullptr;
	Boss* boss_ = nullptr;
	Player* player_ = nullptr;
	Tornado* tornado_ = nullptr;
	BossBulletLump* bossBulletLump_;

	class SceneTransition* sceneTransition_ = nullptr;
	class DeadEffect* deadEffect_ = nullptr;

};