#pragma once
#include <GameObjectManager.h>
#include <BaseGameObject.h>
#include <Sprite.h>

class SinWaveDrawer;


class Enemy : public BaseGameObject {
public:
	Enemy() { CreateTag(this); }
	~Enemy() {}

	void Initialize() override;
	void Update()override;
	void LastUpdate()override;

	void Draw() override;
	void FrontSpriteDraw()override;

	void Debug()override;

	void SetWave(SinWaveDrawer* wave);

	void SetHeartBreak(bool is) { isHeartBreak = is; }

	bool IsDead() { return isDead; }

	bool IsHeartBreak() { return isHeartBreak; }

	float CalculateTangentAngle(float A, float B, float x);

	// 速度ベクトルを回転
	Vector3 RotateVelocity(Vector3 velocity, float angle);

	// 波の傾きに合わせて速度を回転
	Vector3 AdjustVelocityToWave(Vector3 velocity, float A, float B, float x);

private:

	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Sprite> deadSprite_;
	SinWaveDrawer* sinWave_;

	float addlambda = 0.0f;
	float beforlambda = addlambda;

	Vector3 pos{ 1280.0f,360.0f,0.0f };
	Vector3 beforPos = pos;

	float xAccel = 0.0f;
	float addAccel = 0.02f;
	float addDecel = 0.005f;

	bool isJump = false;
	bool isfly = false;
	bool isDamage = false;
	bool isDecele = false;

	float speed = -0.1f;
	Vector3 velo = { -0.1f,0.0f,0.0f };
	Vector3 flyspeed = { 0.0f,0.0f,0.0f };

	float tangent = 0;

	float amplitude = 100.0f;
	float frequency = 0.02f;
	float offsetY = 360.0f;
	float deceleRate = 0.15f;
	float maxAcceleAmp = 400.0f;
	float addLambdaCount = 0;


	bool isMaybeDead = false;
	bool isDead = false;
	int deadTime = 30;


	int roopCount = 0;
	bool isHeartBreak = false;
};
