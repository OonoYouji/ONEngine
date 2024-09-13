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

	void CalculationGage();

private:
	std::vector<std::unique_ptr<Sprite>> hpSprites_;
	uint32_t currentHP_;
	uint32_t maxHP_;
	Vec3 offset_{};
	float distance_;
	bool fluctuationHP_ = false;

	class SinWaveDrawer* pWave_ = nullptr;
	float animationTime_ = 0.0f;


	/// =======================================
	/// ゲージ版の HP 表示
	/// =======================================

	std::vector<std::unique_ptr<Sprite>> frameSprite_;
	std::unique_ptr<Sprite> gaugeSprite_;

	float maxGauge_;
	float currentGauge_;

	Vec3 gaugeOffset_{};

	Vec4 frameColor_{ 1,1,1,1 };
	Vec4 gaugeColor_{ 1,1,1,1 };

};