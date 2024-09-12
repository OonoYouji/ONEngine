#pragma once
#include <GameObjectManager.h>
#include <BaseGameObject.h>
#include <AudioManager.h>
#include <Sprite.h>
#include "Particle/Effect.h"

class SinWaveDrawer;


class Enemy : public BaseGameObject {
public:
	Enemy() { CreateTag(this); }
	~Enemy();

	void Initialize() override;
	void Update() override;
	void LastUpdate() override;

	void Draw() override;
	void FrontSpriteDraw()override;

	void Debug()override;

	void SetWave(SinWaveDrawer* wave);

	void SetHeartBreak(bool is) { isHeartBreak = is; }

	bool IsDead() { return isDead; }
	bool IsCombo() { return isCombo; }
	bool IsHeartBreak() { return isHeartBreak; }
	bool IsScore() { return isScore; }

	void SetIsCombo(bool is) { isCombo = is; }
	void SetIsScore(bool is) { isScore = is; }

	float CalculateTangentAngle(float A, float B, float x);

	// 速度ベクトルを回転
	Vector3 RotateVelocity(Vector3 velocity, float angle);

	// 波の傾きに合わせて速度を回転
	Vector3 AdjustVelocityToWave(Vector3 velocity, float A, float B, float x);

	/*void EffectOccurrence(int effectNum);*/
	Vector3 GetPos() { return pos; }
	float GetHighPoint() { return highPoint; }
	void CalHighPoint();

private:

	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Sprite> deadSprite_;
	SinWaveDrawer* sinWave_;

	float addlambda = 0.0f;
	float beforlambda = addlambda;

	Vector3 pos{ 1280.0f,300.0f,0.0f };
	Vector3 beforPos = pos;
	Vector3 currentSize_{};

	float xAccel = 0.0f;
	float addAccel = 0.2f;
	float addDecel = 0.005f;
	float acceleTime = 0.1f;

	bool isMaybeJump = true;
	bool isJump = false;
	bool isfly = true;
	bool isDamage = false;
	bool isDecele = false;

	float speed = -0.15f;
	Vector3 velo = { -0.1f,0.0f,0.0f };
	Vector3 flyspeed = { -4.0f,-3.0f,0.0f };

	float tangent = 0;

	float amplitude = 100.0f;
	float frequency = 0.02f;
	float offsetY = 360.0f;
	float deceleRate = 0.15f;
	float maxAcceleAmp = 400.0f;
	float addLambdaCount = 0;
	float canJumpAccele = 0.05f;


	bool isBorn = true;
	bool isMaybeDead = false;
	bool isDead = false;
	bool isCombo = false;
	int deadTime = 60;


	int roopCount = 0;
	bool isHeartBreak = false;

	bool isScore = false;
	float highPoint = 1000;


	Effect* AcceleEffect_;
	Effect* deathEffect_;
	//AudioSource* deathSound_;

	/// 効果音
	AudioSource* stampSE_ = nullptr;
	AudioSource* deathSE_ = nullptr;
	AudioSource* catchSE_ = nullptr;

};
