#include "Enemy.h"
#include "Drawer/LineDrawer2D/SinWaveDrawer.h"
#include <ImGuiManager.h>

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::Initialize()
{

	sprite_.reset(new Sprite());
	sprite_->Initialize("uvChecker", "uvChecker.png");

	beforlambda = addlambda;

}

void Enemy::Update()
{

	ImGui::Begin("Setting");
	
	if (ImGui::Button("acce")) // 横の加速値をリセット
	{
		xAccel = 0.0f;
	}
	ImGui::DragFloat("Pos", &pos.x, 1.0f); // イラストの位置
	ImGui::End();

	beforPos = pos;
	beforlambda = addlambda;
	isJump = true;

	pos.x -= (0.02f + xAccel);


	amplitude = sinWave_->GetAmplitude();
	frequency = sinWave_->GetFrequency();
	offsetY = sinWave_->GetOffset();
	addlambda = sinWave_->GetAddLambda();

	if (beforlambda != addlambda)
	{
		isJump = false;
	}

	if (!isfly)
	{
		pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
	}
	else
	{
		flyspeed.y += 0.2f;
		pos.y += flyspeed.y;
		if (pos.y >= amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY)
		{
			pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
			isfly = false;
		}
	}

	sprite_->SetPos(pos);
	if (-(amplitude)+10 >= (pos.y - offsetY) && xAccel > 2.0f && isJump)
	{
		if (!isfly)
		{
			isfly = true;
			flyspeed = pos - beforPos;
		}
	}

	if (!isfly)
	{
		if (beforPos.y < pos.y)
		{
			if (beforlambda != addlambda)
			{
				if (beforlambda > addlambda)
				{
					xAccel += 0.02f;
				}
			}
			else
			{
				xAccel += 0.02f;
			}
		}
	}

	if (pos.x < 0)
	{
		pos.x = 1280;
	}

}

void Enemy::Draw()
{
}

void Enemy::FrontSpriteDraw()
{
	sprite_->Draw();
}

void Enemy::SetWave(SinWaveDrawer* wave)
{
	sinWave_ = wave;
}
