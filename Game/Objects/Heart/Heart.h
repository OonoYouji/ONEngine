#pragma once

#include <GameObjectManager.h>
#include "Particle//Effect.h"


class Heart final : public BaseGameObject {
public:

	Heart() { CreateTag(this); }
	~Heart() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

	bool GetFluctuationHP() const { return fluctuationHP_; }
private:
	class HeartBottom* bottom_ = nullptr;
	class HeartAbove* above_ = nullptr;

	/// ダメージ演出
	Effect* damageEffect_;
	float damegedTime_ = 1.0f;
	bool isDameged_ = false;

	int appearCount_ = 0;
	class PlayerHP* pPlayerHP_ = nullptr;
	bool fluctuationHP_ = false;
};


class HeartBottom final : public BaseGameObject {
public:
	HeartBottom() { CreateTag(this); }
	~HeartBottom() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Debug() override;
private:
	float animationTime_ = 0.0f;
	float speed_ = 8.0f;
	float amplitude_ = 0.05f;
	Model* model_ = nullptr;
	Material flashingMaterial_{};
	bool useFlashingMaterial_ = false;
	float flashingTime_ = 0.2f;
	/// other class pointer
	class SinWaveDrawer* pSinWaveDrawer_ = nullptr;
	class AudioSource* sinon_ = nullptr;
	bool isPlaying_ = false;
};


class HeartAbove final : public BaseGameObject {
public:
	HeartAbove() { CreateTag(this); }
	~HeartAbove() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;
private:
	Model* model_ = nullptr;
	Material flashingMaterial_{};
	bool useFlashingMaterial_ = false;
	float flashingTime_ = 0.2f;
};