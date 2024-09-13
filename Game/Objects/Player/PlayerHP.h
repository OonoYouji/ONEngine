#pragma once

#include <memory>
#include <vector>

#include <GameObjectManager.h>
#include <AudioManager.h>
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
	bool GetHPHeal();

	void CalculationGage();

	void EvalsUpdate();

private:
	uint32_t currentHP_;
	uint32_t maxHP_;
	Vec3 offset_{};
	float distance_;
	bool fluctuationHP_ = false;
	bool healHP_ = false;

	/// =======================================
	/// ゲージ版の HP 表示
	/// =======================================

	std::vector<std::unique_ptr<Sprite>> frameSprite_;
	std::unique_ptr<Sprite> gaugeSprite_;

	float maxGauge_;
	float currentGauge_;
	float baseGauge_;

	int currentDamegeIndex_;

	Vec4 frameColor_{ 1,1,1,1 };
	Vec4 gaugeColor_{ 1,1,1,1 };


	/// デカい、チョーデカいを出すやつ
	enum SIZE_TYPE {
		BIG, SUPER_BIG
	};
	std::vector<std::string> texNames_;
	std::vector<std::string> filePaths_;
	std::vector<Vec2> texSizes_;
	std::vector<float> posYs_;

	float evalLifeTime_ = 1.0f; /// Eval::lifeTimeの初期化用

	struct Eval {
		std::unique_ptr<Sprite> sprite;
		float lifeTime = 0.2f; // 描画する残り時間
		float posY = 0.0f;
		int positionYsIndex = 0;
	};
	std::list<Eval> evals_;

	AudioSource* evalSEs_[2];
 
	AudioSource* deathSE_ = nullptr;
};