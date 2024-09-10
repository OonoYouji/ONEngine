#include "Enemy.h"

#include <ImGuiManager.h>
#include "Drawer/LineDrawer2D/SinWaveDrawer.h"


void Enemy::Initialize()
{
	SetWave(dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer")));

	sprite_.reset(new Sprite());
	sprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	sprite_->SetSize({ 20,20 });
	sprite_->SetColor({ 0.184f, 0.851f, 0.137f, 1.0f });
	deadSprite_.reset(new Sprite());
	deadSprite_->Initialize("enemy_stamp", "enemy_stamp.png");
	deadSprite_->SetSize({ 20,20 });
	deadSprite_->SetColor({ 0.184f, 0.851f, 0.137f ,1.0f });

	beforlambda = addlambda;
	addLambdaCount = sinWave_->GetAddLabdaCount();
}

void Enemy::Update()
{

	if (!isDead) {
		beforPos = pos;
		beforlambda = addlambda;
		if (isMaybeJump)
		{
			isJump = true;
		}

		// sinwaveから変数の値をとる
		amplitude = sinWave_->GetAmplitude();
		frequency = sinWave_->GetFrequency();
		offsetY = sinWave_->GetOffset();
		addlambda = sinWave_->GetAddLambda();
		offsetY -= 20.0f;

		if (beforlambda != addlambda)
		{
			isJump = false;
		}


		// 敵の移動(波に乗ってる時と、そらを飛ぶ)
		if (!isfly)
		{

			tangent = CalculateTangentAngle(amplitude, frequency, (pos.x + addlambda));

			pos.x += speed - xAccel;

			pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
			if (!(-(amplitude)+10.0f >= (pos.y - offsetY)))
			{
				isMaybeJump = true;
			}
		}
		else
		{
			if (-(amplitude) >= (pos.y - offsetY))
			{
				isDamage = true;
			}
			if (pos.y <= 40)
			{
				isMaybeDead = true;
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
					if (!(-(amplitude)+10.0f >= (pos.y - offsetY)))
					{
						isDamage = false;
						isDecele = true;
					}
					else
					{
						isJump = false;
						isMaybeJump = false;
					}
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

		sprite_->SetAngle(tangent);
		deadSprite_->SetAngle(tangent);
		sprite_->SetPos(pos);
		if (amplitude > 25.0f && -(amplitude)+10.0f >= (pos.y - offsetY) && xAccel > 2.0f && isJump)
		{
			if (!isfly)
			{
				isfly = true;
				flyspeed = pos - beforPos;
				flyspeed.y = flyspeed.y * 2;
				if (amplitude >= 40)
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
			if (true)
			{
				if (beforlambda == addlambda)
				{
					if (amplitude * sinf(frequency * ((pos.x - 4) + addlambda)) + offsetY > pos.y &&
						amplitude * sinf(frequency * ((pos.x + 4) + addlambda)) + offsetY < pos.y)
					{
						float t = amplitude / maxAcceleAmp;
						if (t >= 1.0f)
						{
							t = 1.0f;
						}
						xAccel += addAccel * t;
					}
				}
			}
			if (true)
			{
				if (beforlambda == addlambda)
				{
					if (amplitude * sinf(frequency * ((pos.x - 4) + addlambda)) + offsetY < pos.y &&
						amplitude * sinf(frequency * ((pos.x + 4) + addlambda)) + offsetY > pos.y)
					{
						float t = amplitude / maxAcceleAmp;
						if (t >= 1.0f)
						{
							t = 1.0f;
						}
						xAccel -= addDecel * t;
					}
				}
			}
		}

		if (pos.x < 0)
		{
			if (roopCount >= 0)
			{
				pos.x = 1280;
				roopCount++;
				sprite_->SetColor({ 0.8667f, 0.1020f, 0.1294f, 1.0f });
				deadSprite_->SetColor({ 0.8667f, 0.1020f, 0.1294f, 1.0f });
			}
			else if (roopCount == 100)
			{
				isHeartBreak = true;
				isDead = true;
				roopCount = 99;
			}
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
		ImGui::DragFloat("AddAccel", &addAccel, 0.001f, 0.0f, 0.5f);
		ImGui::DragFloat("AddDecel", &addDecel, 0.001f, 0.0f, 0.5f);
		if (ImGui::Button("acce")) // 横の加速値をリセット
		{
			xAccel = 0.0f;
		}
		ImGui::DragFloat("Pos", &pos.x, 1.0f); // イラストの位置
		ImGui::Text("%f", velo.x); ImGui::SameLine();
		ImGui::Text("%f", velo.y);
		ImGui::Text("%f", xAccel);
		ImGui::DragFloat("MaxAcceleAmp", &maxAcceleAmp, 0.1f, 1.0f, 400.0f);
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

void Enemy::EffectOccurrence(int effectNum)
{


}
