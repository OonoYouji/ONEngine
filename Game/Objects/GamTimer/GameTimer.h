#pragma once

#include <memory>

#include <GameObjectManager.h>
#include <Sprite.h>


class GameTimer : public BaseGameObject {
public:
	GameTimer() { CreateTag(this); }
	~GameTimer() {}

	void Initialize() override;
	void Update() override;
	void BackSpriteDraw() override;
	void Debug() override;

	void SetMaxTime(float maxTime) { 
		maxTime_ = maxTime; 
	}
private:
	std::unique_ptr<Sprite> sprite_;
	Vec4 color_{ 0,20.0f / 255.0f,0,1 };
	float maxTime_ = 0.0f;
	float currentTime_ = 0.0f;
};