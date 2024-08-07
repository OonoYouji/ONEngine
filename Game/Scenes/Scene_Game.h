#pragma once

#include <BaseScene.h>

#include <memory>

#include <Model.h>
#include <Vector3.h>
#include <Sprite.h>
#include <Transform.h>

#include <AudioManager.h>

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

	std::unique_ptr<AudioSource> audioSource_;

};