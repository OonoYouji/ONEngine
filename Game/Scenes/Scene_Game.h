#pragma once

#include <BaseScene.h>

#include <Model.h>
#include <Vector3.h>

/// ===================================================
/// ゲームシーン
/// ===================================================
class Scene_Game final : public BaseScene {
public:

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:


	Model* model_;
	Vec3 rotate_{};

};