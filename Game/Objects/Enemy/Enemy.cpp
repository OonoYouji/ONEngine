#include "Enemy.h"
#include "Drawer/LineDrawer2D/SinWaveDrawer.h"
#include <ImGuiManager.h>


void Enemy::Initialize()
{

	sprite_.reset(new Sprite());
	sprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	sprite_->SetSize({ 20,20 });
	deadSprite_.reset(new Sprite());
	deadSprite_->Initialize("SINON_enemy_stanp", "SINON_enemy_stanp.png");
	deadSprite_->SetSize({ 20,20 });

	beforlambda = addlambda;

}

void Enemy::Update()
{

	if (!isDead) {
		beforPos = pos;
		beforlambda = addlambda;
		isJump = true;

		amplitude = sinWave_->GetAmplitude();
		frequency = sinWave_->GetFrequency();
		offsetY = sinWave_->GetOffset();
		addlambda = sinWave_->GetAddLambda();
		offsetY -= 10.0f;

		if (beforlambda != addlambda)
		{
			isJump = false;
		}

		float tangent = 0;
		if (!isfly)
		{

			tangent = CalculateTangentAngle(amplitude, frequency, pos.x);
#ifdef _DEBUG
			ImGui::Begin("theta");
			ImGui::Text("%f", tangent);
			ImGui::End();
#endif // _DEBUG

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
				if (amplitude <= 20)
				{
					if (isMaybeDead)
					{
						isDead = true;
					}
				}
				isMaybeDead = false;
			}
		}

		sprite_->SetPos(pos);
		if (amplitude > 25.0f && -(amplitude)+10.0f >= (pos.y - offsetY) && xAccel > 2.0f && isJump)
		{
			if (!isfly)
			{
				isfly = true;
				flyspeed = pos - beforPos;
				if (amplitude >= 60)
				{
					isMaybeDead = true;
				}
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

}

void Enemy::LastUpdate()
{
	if (isDead)
	{
		if (deadTime <= 0)
		{
			Destory();
		}
		else
		{
			deadTime--;
			amplitude = sinWave_->GetAmplitude();
			frequency = sinWave_->GetFrequency();
			offsetY = sinWave_->GetOffset();
			addlambda = sinWave_->GetAddLambda();
			pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
			deadSprite_->SetPos(pos);
		}
	}
}

void Enemy::Draw()
{
}

void Enemy::FrontSpriteDraw()
{
	if (!isDead)
	{
		sprite_->Draw(2);
	}
	else
	{
		deadSprite_->Draw(2);
	}
}

void Enemy::Debug()
{
	if (ImGui::TreeNodeEx("Enemy", ImGuiTreeNodeFlags_DefaultOpen))
	{
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

		ImGui::TreePop();
	}
	
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
