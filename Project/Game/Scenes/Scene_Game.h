#pragma once

#include"Game/Scenes/Manager/BaseScene.h"

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
	class DemoObject* demoObjA_ = nullptr;
	class DemoObject* demoObjB_ = nullptr;

	size_t entityA_;
	size_t entityB_;
};