#pragma once
#include "LineDrawer2D.h"
#include <vector>
#include <Sprite.h>
#include <Transform.h>
#include <BaseGameObject.h>

class SinWaveDrawer : public BaseGameObject
{
public:
	SinWaveDrawer();
	~SinWaveDrawer();

	void Initialize()override;
	
	void Update()override;

	void Draw()override;

	void FrontSpriteDraw() override;

	//enemyに渡す
	float GetAmplitude();
	float GetFrequency();
	float GetOffset();
	float GetAddLambda();

private:


	Vector3 pos{ 1280.0f,360.0f,0.0f };
	Vector3 beforPos = pos;

	std::vector<float> waveHeights_;
	float separateLenght_ = 1;
	float addlambda = 0.0f;
	float beforlambda = addlambda;


	float xAccel = 0.1f;
	bool isJump = false;
	bool isfly = false;

	Vector3 velo = { 0.02f,0.0f,0.0f };
	Vector3 flyspeed = { 0.0f,0.0f,0.0f };


	int screenOfDivisions = 200;
	float amplitude = 100.0f;
	float frequency = 0.02f;
	float offsetY = 360.0f;
	float addPadAmp = 2.0f;
	float addPadLam = 2.0f;
	float addNaturalAmp = 2.0f;
};
