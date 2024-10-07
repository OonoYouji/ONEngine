#pragma once

#include <BaseScene.h>
#include"Quaternion.h"
#include"Matrix3x3.h"
#include"Vector3.h"

#include "Player/Player.h"
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

	Player* player_ = nullptr;
};