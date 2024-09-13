#include "Enemy.h"

#include <ImGuiManager.h>
#include <WorldTime.h>
#include "Drawer/LineDrawer2D/SinWaveDrawer.h"
#include "Camera/GameCamera.h"
#include "Shake/Shake.h"

Enemy::~Enemy() {

	deathEffect_->Destory();
	AcceleEffect_->Destory();
}

void Enemy::Initialize() {
	SetWave(dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer")));

	sprite_.reset(new Sprite());
	sprite_->Initialize("medicine", "medicine.png");
	defaultSize = { 20,40,0 };
	sprite_->SetSize({ 20,40 });
	currentSize_ = { 20,40,0 };
	sprite_->SetColor({ 0.184f, 0.851f, 0.137f, 1.0f });
	deadSprite_.reset(new Sprite());
	deadSprite_->Initialize("medicine_stamp", "medicine_stamp.png");
	deadSprite_->SetSize({ 20,40 });
	deadSprite_->SetColor({ 0.184f, 0.851f, 0.137f ,1.0f });

	//deathSound_ = new AudioSource();
	//deathSound_->SetAudioClip("enemyDeath.wav");

	beforlambda = addlambda;
	addLambdaCount = sinWave_->GetAddLabdaCount();

	deathEffect_ = new Effect();
	deathEffect_->SetGrainMode(1);
	deathEffect_->Initialize();
	deathEffect_->SetOverType(0);
	deathEffect_->OverTimeSetting(8, 4);
	deathEffect_->SetVariavles2D({ 0.0f,0.0f,0.0f }, -6.0f, { 5.0f,5.0f,0.0f }, 30, true, true, 8.0f, 10.0f);
	deathEffect_->ShapeType(1);
	deathEffect_->ShiftingSpeedType(1);
	deathEffect_->ShiftSpeedSetting(0.9f);


	AcceleEffect_ = new Effect();
	AcceleEffect_->SetGrainMode(1);
	AcceleEffect_->Initialize();
	AcceleEffect_->SetOverType(1);
	AcceleEffect_->SetVariavles2D({ 0.0f,0.0f,0.0f }, -2.0f, { 5.0f,5.0f,0.0f }, 20, true, true, 10.0f, 14.0f);
	AcceleEffect_->SizeChangeSetting(true, true, false, { 1.0f,1.0f,1.0f });
	AcceleEffect_->ShapeType(0);

	stampSE_ = new AudioSource;
	stampSE_->SetAudioClip("EnemyStamp.wav");

	deathSE_ = new AudioSource;
	deathSE_->SetAudioClip("EnemyDeath.wav");

	catchSE_ = new AudioSource;
	catchSE_->SetAudioClip("kasoku.wav");

	if (isBreakType_) {
		sprite_->SetColor({ 0.988f, 0.784f, 0.0f, 1.0f });
	}
}

void Enemy::Update() {

	if (!isDead) {
		beforPos = pos;
		beforlambda = addlambda;
		if (isMaybeJump) {
			isJump = true;
		}

		// sinwaveから変数の値をとる
		amplitude = sinWave_->GetAmplitude();
		frequency = sinWave_->GetFrequency();
		offsetY = sinWave_->GetOffset();
		addlambda = sinWave_->GetAddLambda();

		addAccel = baseAccele;
		/*if (beforlambda < addlambda) {
			isJump = false;
		}*/
		if (beforlambda > addlambda) {
			addAccel = 1.0f;
		}

		// 敵の移動(波に乗ってる時と、そらを飛ぶ)
		if (!isfly) {
			currentSize_ = Vector3::Lerp(currentSize_, defaultSize, 0.15f);
			sprite_->SetSize({ currentSize_.x,currentSize_.y });
			tangent = CalculateTangentAngle(amplitude, frequency, (pos.x + addlambda));

			pos.x += (velo.x - xAccel) * (WorldTime::DeltaTime() * 60.0f);

			pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
			if (!(-(amplitude)+10.0f >= (pos.y - offsetY))) {
				isMaybeJump = true;
			}
		}
		else {
			if (AcceleEffect_->GetStart()) {
				AcceleEffect_->EffectStop();
			}
			currentSize_ = Vector3::Lerp(currentSize_, { defaultSize.x - 5.0f,defaultSize.y + 5.0f,0 }, 0.15f);
			sprite_->SetSize({ currentSize_.x,currentSize_.y });
			if (-(amplitude)-5.0f >= (pos.y - offsetY)) {
				isDamage = true;
			}
			if (pos.y <= 40) {
				isMaybeDead = true;
			}
			flyspeed.y += (0.2f) * (WorldTime::DeltaTime() * 60.0f);
			pos += (flyspeed) * (WorldTime::DeltaTime() * 60.0f);
			CalHighPoint();

			if (pos.y >= amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY) {
				pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
				isfly = false;


				if (!isBorn) {
					if (amplitude <= 4) {
						if (isMaybeDead || isDamage) {
							AcceleEffect_->EffectStop();
							//isDead = true;
							isCombo = true;
							isScore = true;
							isDamage = false;
							/*deathSound_->PlayAudio();*/
							BaseCamera* nowCamera = CameraManager::GetInstance()->GetCamera("GameCamera");
							nowCamera->GetShake()->SetStartTime(0.2f);
							nowCamera->GetShake()->Start();
						}
					}
				}
				else {
					isBorn = false;
					isDamage = false;
				}
				isMaybeDead = false;


				if (isDamage &&
					amplitude * sinf(frequency * ((pos.x - 4) + addlambda)) + offsetY < pos.y &&
					amplitude * sinf(frequency * ((pos.x + 4) + addlambda)) + offsetY > pos.y) {

					currentSize_ = { 25.0f,35.0f,0 };
					if (!(-(amplitude)+10.0f >= (pos.y - offsetY))) {
						isDecele = true;
					}
					else {
						isJump = false;
						isMaybeJump = false;
					}
					if (isBreakType_) {
						isDead = true;
						isHeartBreak = true;
					}
				}
				else if (isDamage &&
					amplitude * sinf(frequency * ((pos.x - 4) + addlambda)) + offsetY > pos.y &&
					amplitude * sinf(frequency * ((pos.x + 4) + addlambda)) + offsetY < pos.y) {

					///
					/// キャッチしたときはここ
					///
					catchSE_->PlayAudio();

					if (-(amplitude)+10.0f >= (pos.y - offsetY)) {
						isJump = false;
						isMaybeJump = false;
					}

				}
				isDamage = false;
			}
		}

		sprite_->SetAngle(tangent);
		deadSprite_->SetAngle(tangent);
		sprite_->SetPos(pos);
		if (amplitude > 4.0f && -(amplitude)+4.0f >= (pos.y - offsetY) && xAccel > canJumpAccele && isJump && amplitude != 50) {
			if (!isfly) {
				isfly = true;
				float jumpMulti = 1.0f;
				switch (jumpCount_) {
				case 0:
					jumpMulti = 1.75f;
					jumpCount_++;
					/*defaultSize = { 25,50,0 };
					sprite_->SetSize({ 25,50 });*/
					break;
				case 1:
					jumpMulti = 2.0f;
					jumpCount_++;
					/*defaultSize = { 30,60,0 };
					sprite_->SetSize({ 30,60 });*/
					break;
				case 2:
					jumpMulti = 3.25f;
					/*defaultSize = { 35,70,0 };
					sprite_->SetSize({ 35,70 });*/
					break;
				}
				flyspeed = pos - beforPos;
				flyspeed = flyspeed.Normalize() * (xAccel + -velo.x);
				if (flyspeed.x < -2.0f) {
					flyspeed.x = -2.0f;
				}
				if (flyspeed.y < -2.8f) {
					flyspeed.y = -2.8f;
				}
				flyspeed.y = flyspeed.y * jumpMulti;
				if (amplitude >= 40) {
					isMaybeDead = true;
				}
			}
		}

		if (isDecele) {
			xAccel = std::lerp(xAccel, 0.0f, deceleRate);
			if (xAccel < 0.2f) {
				isDecele = false;
				xAccel = 0;
			}
		}

		if (!isfly) {

			if (amplitude * sinf(frequency * ((pos.x - 4) + addlambda)) + offsetY > pos.y &&
				amplitude * sinf(frequency * ((pos.x + 4) + addlambda)) + offsetY < pos.y) {
				AcceleEffect_->EffectStart();
				acceleTime += (0.01f) * (WorldTime::DeltaTime() * 60.0f);
				float t = amplitude / maxAcceleAmp;
				if (t >= 1.0f) {
					t = 1.0f;
				}
				if (!isBreakType_) {
					xAccel += ((addAccel * (acceleTime * acceleTime)) * t) * (WorldTime::DeltaTime() * 60.0f);
				}
				else {
					xAccel += (((addAccel * (acceleTime * acceleTime)) * t) * 0.5f) * (WorldTime::DeltaTime() * 60.0f);
				}
				if (beforPos.y > pos.y) {
					AcceleEffect_->EffectStop();
				}
			}


			if (amplitude * sinf(frequency * ((pos.x - 4) + addlambda)) + offsetY < pos.y &&
				amplitude * sinf(frequency * ((pos.x + 4) + addlambda)) + offsetY > pos.y) {
				AcceleEffect_->EffectStop();
				acceleTime = 0.3f;
				/*float t = amplitude / maxAcceleAmp;
				if (t >= 1.0f) {
					t = 1.0f;
				}
				xAccel -= (addDecel * t) * (WorldTime::DeltaTime() * 60.0f);*/
			}


		}


		if (pos.x < 0) {
			if (roopCount == 0) {
				isDead = true;
				isScore = true;
				pos.x = -30.0f;
				roopCount++;
				sprite_->SetColor({ 0.8667f, 0.1020f, 0.1294f, 1.0f });
				deadSprite_->SetColor({ 0.8667f, 0.1020f, 0.1294f, 1.0f });
			}
			/*else if (roopCount == 1) {
				isHeartBreak = true;
				isDead = true;
				roopCount = 99;
			}*/
		}
	}

	deathEffect_->SetPos({ pos.x,pos.y });
	AcceleEffect_->SetPos({ pos.x,pos.y });
}

void Enemy::LastUpdate() {
	if (isDead) {
		if (deadTime <= 0) {
			///
			/// 死んだときはここ
			/// 
			Destory();
			
		}
		else {
			deadTime -= (WorldTime::DeltaTime() * 60.0f);
			amplitude = sinWave_->GetAmplitude();
			frequency = sinWave_->GetFrequency();
			offsetY = sinWave_->GetOffset();
			addlambda = sinWave_->GetAddLambda();
			pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
			deadSprite_->SetPos(pos);
			if (deadTime <= 58 && deadTime >= 57) {
				AcceleEffect_->EffectStop();
			}
			if (deadTime <= 55 && deadTime >= 54) {
				///
				/// つぶれた時はここ
				/// 
				
			}
			if (deadTime <= 10 && deadTime >= 9) {
				
			}
		}
	}
}

void Enemy::Draw() {}

void Enemy::FrontSpriteDraw() {
	if (!isDead) {
		sprite_->Draw(2);
	}
	else {
		deadSprite_->Draw(2);
	}
}

void Enemy::Debug() {
	if (ImGui::TreeNodeEx("Enemy", ImGuiTreeNodeFlags_DefaultOpen)) {
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
		ImGui::DragFloat("Can Jump Accel", &canJumpAccele, 0.01f, 0.01f, 4.0f);
		ImGui::Separator();
		ImGui::Text("%f", flyspeed.x); ImGui::SameLine();
		ImGui::Text("%f", flyspeed.y);
		if (ImGui::Button("EffectStart")) {
			deathEffect_->EffectStart();
		}
		if (ImGui::Button("EffectStop")) {
			deathEffect_->EffectStop();
		}
		ImGui::TreePop();
	}
}

void Enemy::SetWave(SinWaveDrawer* wave) {
	sinWave_ = wave;
}

float Enemy::CalculateTangentAngle(float A, float B, float x) {
	float dy_dx = A * B * cosf(B * x);  // 勾配
	return atan2f(dy_dx, 1.0f);
}

Vector3 Enemy::RotateVelocity(Vector3 velocity, float angle) {
	float cosTheta = cosf(angle);
	float sinTheta = sinf(angle);

	// 回転行列を適用
	Vector3 rotatedVelocity;
	rotatedVelocity.x = cosTheta * velocity.x - sinTheta * velocity.y;
	rotatedVelocity.y = sinTheta * velocity.x + cosTheta * velocity.y;
	rotatedVelocity.z = 0.0f;

	return rotatedVelocity;
}

Vector3 Enemy::AdjustVelocityToWave(Vector3 velocity, float A, float B, float x) {
	// 現在のx座標に基づいて波の接線角度を計算
	float tangentAngle = CalculateTangentAngle(A, B, x);

	// 速度を回転して波の接線方向に合わせる
	return RotateVelocity(velocity, tangentAngle);
}

//void Enemy::EffectOccurrence(int effectNum) {
//
//
//}

void Enemy::CalHighPoint() {

	if (!isBorn) {
		float newHighPoint = pos.y;

		if (highPoint > newHighPoint) {
			highPoint = pos.y;
		}

		if (highPoint < 250) {
			if (isSizeChangeThree) {
				defaultSize = { 40,80,0 };
				sprite_->SetSize({ 40,80 });
				medicSize_++;
				isSizeChangeThree = false;
			}
		}
		else if (highPoint < 300) {
			if (isSizeChangeTwo) {
				defaultSize = { 30,60,0 };
				sprite_->SetSize({ 30,60 });
				medicSize_++;
				isSizeChangeTwo = false;
			}
		}
		else if (highPoint < 350) {
			if (isSizeChangeOne) {
				defaultSize = { 25,50,0 };
				sprite_->SetSize({ 20,50 });
				medicSize_++;
				isSizeChangeOne = false;
			}
		}
	}

}
