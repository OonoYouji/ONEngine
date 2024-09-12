#pragma once

#include <memory>
#include <vector>

#include <GameObjectManager.h>
#include <Sprite.h>

class PlayerHP final : public BaseGameObject {
public:

	PlayerHP() { CreateTag(this); }
	~PlayerHP() {}

	void Initialize() override;
	void Update() override;
	void FrontSpriteDraw() override;
	void Debug() override;

	bool GetHPFluctuation();

private:
	std::vector<std::unique_ptr<Sprite>> hpSprites_;
	uint32_t currentHP_;
	uint32_t maxHP_;
	Vec3 offset_{};
	float distance_;
	bool fluctuationHP_ = false;

	class SinWaveDrawer* pWave_ = nullptr;
	float animationTime_ = 0.0f;

};