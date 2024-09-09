#pragma once

#include <GameObjectManager.h>

class GameCameraState final : public BaseGameObject {
public:

	GameCameraState() { CreateTag(this); }
	~GameCameraState() {}

	void Initialize() override;
	void Update() override;
	void Debug() override;

private:

	Vec3 positions_[3];
	Vec3 rotates_[3];
	float moveTimes_[3][3];

	int currentSceneId_;
	int prevSceneId_;

	class SceneManager* pSceneManager_ = nullptr;

};