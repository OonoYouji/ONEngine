#pragma once

#include"Game/Scenes/Manager/BaseScene.h"

#include "EntityConponentSystem/System/MoveSystem.h"

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
	class DemoObject* demoObj_ = nullptr;

	size_t entity_;
};