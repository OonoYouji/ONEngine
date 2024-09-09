#pragma once

#include <BaseScene.h>

/// ===================================================
/// ゲームシーン
/// ===================================================
class Scene_Game final : public BaseScene {
public:
	Scene_Game() {}
	~Scene_Game() {}

	void Initialize() override;
	void Update() override;
	void Draw() override {};
private:

};