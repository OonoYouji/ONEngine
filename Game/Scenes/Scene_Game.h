#pragma once

#include <BaseScene.h>

#include <memory>

#include <Model.h>
#include <Vector3.h>
#include <Sprite.h>
#include <Transform.h>

#include <AudioManager.h>
#include "LineDrawer2D/LineDrawer2D.h"
#include "LineDrawer2D/SinWaveDrawer.h"
#include "Enemy/Enemy.h"
#include "EnemyManager.h"

/// ===================================================
/// ゲームシーン
/// ===================================================
class Scene_Game final : public BaseScene {
public:

	Scene_Game();
	~Scene_Game();

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:


	Model* model_;
	Transform transform_{};

	std::unique_ptr<Sprite> sprite_;

	SinWaveDrawer* sinWave_;
	Enemy* enemy_;
	EnemyManager* enemyManager_;
};