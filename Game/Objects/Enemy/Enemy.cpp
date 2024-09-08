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

	ImGui::Begin("Enemy");

	ImGui::DragFloat("DeceleRate", &deceleRate, 0.001f, 0.0f, 1.0f);
	if (ImGui::Button("acce")) // 横の加速値をリセット
	{
		xAccel = 0.0f;
	}
	ImGui::DragFloat("Pos", &pos.x, 1.0f); // イラストの位置
	ImGui::Text("%f", velo.x); ImGui::SameLine();
	ImGui::Text("%f", velo.y);
	ImGui::Text("%f", xAccel);
	ImGui::Separator();
	ImGui::Text("%f", flyspeed.x); ImGui::SameLine();
	ImGui::Text("%f", flyspeed.y);
	ImGui::End();

	beforPos = pos;
	beforlambda = addlambda;
	isJump = true;

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

		pos.x += speed - xAccel;

		pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
	}
	else
	{
		if (-(amplitude) >= (pos.y - offsetY))
		{
			isDamage = true;
		}
		flyspeed.y += 0.2f;
		pos += flyspeed;
		if (pos.y >= amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY)
		{
			pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
			isfly = false;


			if (isDamage &&
				amplitude * sinf(frequency * ((pos.x - 4) + addlambda)) + offsetY < pos.y &&
				amplitude * sinf(frequency * ((pos.x + 4) + addlambda)) + offsetY > pos.y)
			{
				isDamage = false;
				isDecele = true;
			}
			isDamage = false;
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

	if (isDecele)
	{
		speed = std::lerp(speed, 0.0f, deceleRate);
		xAccel = 0;
		if (speed > -0.15f)
		{
			isDecele = false;
			xAccel = 0;
		}
	}

	if (!isfly)
	{
		if (beforPos.y < pos.y)
		{
			if (beforlambda == addlambda)
			{
				if (amplitude * sinf(frequency * ((pos.x - 4) + addlambda)) + offsetY > pos.y &&
					amplitude * sinf(frequency * ((pos.x + 4) + addlambda)) + offsetY < pos.y)
				{
					xAccel += 0.02f;
				}
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

float Enemy::CalculateTangentAngle(float A, float B, float x)
{
	float dy_dx = A * B * cosf(B * x);  // 勾配
	return atan2f(dy_dx, 1.0f);
}

Vector3 Enemy::RotateVelocity(Vector3 velocity, float angle)
{
	float cosTheta = cosf(angle);
	float sinTheta = sinf(angle);

	// 回転行列を適用
	Vector3 rotatedVelocity;
	rotatedVelocity.x = cosTheta * velocity.x - sinTheta * velocity.y;
	rotatedVelocity.y = sinTheta * velocity.x + cosTheta * velocity.y;
	rotatedVelocity.z = 0.0f;

	return rotatedVelocity;
}

Vector3 Enemy::AdjustVelocityToWave(Vector3 velocity, float A, float B, float x)
{
	// 現在のx座標に基づいて波の接線角度を計算
	float tangentAngle = CalculateTangentAngle(A, B, x);

	// 速度を回転して波の接線方向に合わせる
	return RotateVelocity(velocity, tangentAngle);
}
