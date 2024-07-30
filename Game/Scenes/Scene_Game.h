#pragma once

#include <BaseScene.h>

#include <memory>

#include <Model.h>
#include <Vector3.h>
#include <Sprite.h>

/// ===================================================
/// ゲームシーン
/// ===================================================
class Scene_Game final : public BaseScene {
public:

	Scene_Game() = default;
	~Scene_Game();

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:


	Model* model_;
	Vec3 rotate_{};

	std::unique_ptr<Sprite> sprite_;

};