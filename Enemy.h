#pragma once
#include <BaseGameObject.h>
#include <Sprite.h>

class SinWaveDrawer;


class Enemy: public BaseGameObject
{
public:
	Enemy();
	~Enemy();

public:

	void Initialize() override;

	void Update()override;

	void Draw() override;

	void FrontSpriteDraw()override;

	void SetWave(SinWaveDrawer* wave);

private:

	std::unique_ptr<Sprite> sprite_;
	SinWaveDrawer* sinWave_;

	float addlambda = 0.0f;
	float beforlambda = addlambda;

	Vector3 pos{ 1280.0f,360.0f,0.0f };
	Vector3 beforPos = pos;

	float xAccel = 0.0f;
	bool isJump = false;
	bool isfly = false;
	bool isDamage = false;
	bool isDecele = false;

	Vector3 velo = { 0.02f,0.0f,0.0f };
	Vector3 flyspeed = { 0.0f,0.0f,0.0f };

	float amplitude = 100.0f;
	float frequency = 0.02f;
	float offsetY = 360.0f;
	float deceleRate = 0.15f;
};
