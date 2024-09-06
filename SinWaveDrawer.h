#pragma once
#include "LineDrawer2D.h"
#include <vector>
#include <Sprite.h>
#include <Transform.h>

class SinWaveDrawer
{
public:
	SinWaveDrawer();
	~SinWaveDrawer();

	void Initialize();
	
	void Update();

	void Draw();

private:

	std::unique_ptr<Sprite> sprite_;
	Vector3 pos{ 1280.0f,360.0f,0.0f };

	std::vector<float> waveHeights_;
	float separateLenght_ = 1;


	int screenOfDivisions = 200;
	float amplitude = 100.0f;
	float frequency = 0.05f;
	float offsetY = 360.0f;

};
