#include "Heart.h"

#include <algorithm>

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <CameraManager.h>
#include <WorldTime.h>
#include <AudioSource.h>

#include "Player/PlayerHP.h"
#include "LineDrawer2D/SinWaveDrawer.h"
#include "Shake/Shake.h"


void Heart::Initialize() {

	drawLayerId = 1;
	transform_.scale = Vec3::kOne * 1.2f;
	transform_.position = Vec3(0.5f, -0.3f, -1.2f);
	transform_.rotate.z = 0.3f;

	bottom_ = new HeartBottom;
	bottom_->Initialize();
	bottom_->drawLayerId = drawLayerId;

	above_ = new HeartAbove;
	above_->Initialize();
	above_->drawLayerId = drawLayerId;

	bottom_->SetParent(this);
	above_->SetParent(this);

	pPlayerHP_ = dynamic_cast<PlayerHP*>(GameObjectManager::GetGameObject("PlayerHP"));

	// エフェクト
	damageEffect_ = new Effect();
	damageEffect_->SetGrainMode(0);
	damageEffect_->Initialize();
	damageEffect_->SetOverType(0);
	damageEffect_->OverTimeSetting(8, 0);
	damageEffect_->SetVariavles({ 0.0f,-0.025f,0.0f }, 0.2f, { 0.4f,0.4f,0.4f }, 30, true, true, 0.2f, 0.3f);
	damageEffect_->ShapeType(2);
	damageEffect_->EffectStart();
	damageEffect_->SetPos(transform_.position);


	healEffect_ = new Effect();
	healEffect_->SetGrainMode(0);
	healEffect_->Initialize();
	healEffect_->SetOverType(0);
	healEffect_->SetHeal(true);
	healEffect_->OverTimeSetting(8, 0);
	healEffect_->SetVariavles({ 0.0f,0.0025f,0.0f }, 0.01f, { 0.4f,0.4f,0.4f }, 30, true, true, 0.2f, 0.3f);
	healEffect_->ShapeType(2);
	healEffect_->SetBoxSize(1.0f, 1.0f);
	healEffect_->EffectStart();
	healEffect_->SetPos(transform_.position);


	damageSE_ = new AudioSource;
	damageSE_->SetAudioClip("glass_hibi.wav");

	healSE_ = new AudioSource;
	healSE_->SetAudioClip("heal.wav");

}

void Heart::Update() {
	if(pPlayerHP_) {

		if(isDameged_) {
			damegedTime_ -= WorldTime::DeltaTime();
			if(damegedTime_ <= 0.0f) {
				isDameged_ = false;
			}
		} else {

			damageEffect_->SetPos(transform_.position);
			fluctuationHP_ = pPlayerHP_->GetHPFluctuation();
			if(fluctuationHP_) {
				damageSE_->PlayAudio();
				isDameged_ = true;
				damegedTime_ = 0.5f;
			}
		}

		if (isHeal_) {
			healTime_ -= WorldTime::DeltaTime();
			if (healTime_ <= 0.0f) {
				healEffect_->EffectStop();
				isHeal_ = false;
			}
		}
		else {

			healEffect_->SetPos({ transform_.position.x,transform_.position.y - 0.3f,transform_.position.z });
			startHeal_ = pPlayerHP_->GetHPHeal();
			if (startHeal_) {
				healSE_->volume = 0.2f;
				healSE_->PlayAudio();
				appearCount_ = 8;
				healEffect_->OverTimeSetting(12, appearCount_);
				healEffect_->EffectStart();
				isHeal_ = true;
				healTime_ = 0.5f;
			}
		}

	}
}

void Heart::Draw() {}



/// ===================================================
/// 心臓の下部
/// ===================================================

void HeartBottom::Initialize() {
	model_ = ModelManager::Load("HeartBottom");
	animationTime_ = 0.0f;
	amplitude_ = 0.6f;

	pSinWaveDrawer_ = dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer"));

	flashingMaterial_.CreateMaterial("white2x2");

	sinon_ = new AudioSource();
	sinon_->SetAudioClip("sinon.wav");

}

void HeartBottom::Update() {
	if(!pSinWaveDrawer_) { return; }

	animationTime_ += WorldTime::DeltaTime() * (pSinWaveDrawer_->GetAmplitude() / 100.0f);

	float sinValue = std::sin(speed_ * animationTime_) * 0.5f + 0.5f;
	float heartBeat = amplitude_ * sinValue;
	transform_.scale = Vec3::kOne + (Vec3::kOne * heartBeat);


	/// 再生していたら
	if(isPlaying_) {
		if(sinValue <= 0.02f) {
			isPlaying_ = false;
		}
	} else { /// 再生していなければ
		if(sinValue >= 0.98f) {
			sinon_->PlayAudio();
			isPlaying_ = true;
		}
	}

	if(useFlashingMaterial_) {
		flashingTime_ -= WorldTime::DeltaTime();
		if(flashingTime_ <= 0.0f) {
			useFlashingMaterial_ = false;
		}
	} else {
		useFlashingMaterial_ = dynamic_cast<Heart*>(GetParent())->GetFluctuationHP();
		flashingTime_ = 0.1f;
		if(useFlashingMaterial_) {
			///
			/// ここでGameCameraをシェイクさせる
			///

			BaseCamera* nowCamera = CameraManager::GetInstance()->GetCamera("GameCamera");
			nowCamera->GetShake()->SetStartTime(0.2f);
			nowCamera->GetShake()->Start();;
		}
	}

}

void HeartBottom::Draw() {
	if(useFlashingMaterial_) {
		model_->Draw(&transform_, &flashingMaterial_);
	} else {
		model_->Draw(&transform_);
	}
}

void HeartBottom::Debug() {
	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("time", &animationTime_, 0.0f);
		ImGui::DragFloat("speed", &speed_, 0.001f);
		ImGui::DragFloat("amplitude", &amplitude_, 0.001f);

		ImGui::TreePop();
	}
}



/// ===================================================
/// 心臓の下部
/// ===================================================

void HeartAbove::Initialize() {
	model_ = ModelManager::Load("HeartAbove");
	flashingMaterial_.CreateMaterial("white2x2");
}

void HeartAbove::Update() {

	Heart* parent = dynamic_cast<Heart*>(GetParent());
	if(!parent) {
		return;
	}

	if(useFlashingMaterial_) {
		flashingTime_ -= WorldTime::DeltaTime();
		if(flashingTime_ <= 0.0f) {
			useFlashingMaterial_ = false;
		}
	} else {
		useFlashingMaterial_ = parent->GetFluctuationHP();
		flashingTime_ = 0.1f;
		if(useFlashingMaterial_) {

		}
	}
}

void HeartAbove::Draw() {
	if(useFlashingMaterial_) {
		model_->Draw(&transform_, &flashingMaterial_);
	} else {
		model_->Draw(&transform_);
	}
}
