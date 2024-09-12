#pragma once

#include <GameObjectManager.h>
#include <Sprite.h>

class GameResult final : public BaseGameObject {
public:

	GameResult() { CreateTag(this); }
	~GameResult() {}

	void Initialize() override;
	void Update() override;

	void BackSpriteDraw() override;

	//void Debug() override;

	void WaveUpdate();

private:

	class Wave* pWave_ = nullptr;
	float waveAnimationTime_ = 0.0f;
	float waveMaxAnimationTime_ = 1.0f;
	float waveAnimationSpeed_ = 20.0f;
	

};