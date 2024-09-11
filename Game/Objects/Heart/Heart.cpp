#include "Heart.h"

#include <algorithm>

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <WorldTime.h>
#include <AudioSource.h>

#include "Player/PlayerHP.h"
#include "LineDrawer2D/SinWaveDrawer.h"


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
	damageEffect_->Initialize();
	damageEffect_->SetGrainMode(0);
	damageEffect_->SetOverType(0);
	damageEffect_->OverTimeSetting(8, 0);
	damageEffect_->SetVariavles({ 0.0f,-0.025f,0.0f }, 0.2f, { 0.4f,0.4f,0.4f }, 30, true, true, 0.2f, 0.3f);
	damageEffect_->ShapeType(2);
	damageEffect_->EffectStart();
	damageEffect_->SetPos(transform_.position);

}

void Heart::Update() {
	if(pPlayerHP_) {

		damageEffect_->SetPos(transform_.position);
		if(pPlayerHP_->GetHPFluctuation()) {
			appearCount_ += 2;
			damageEffect_->OverTimeSetting(12, appearCount_);
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

	sinon_ = new AudioSource();
	sinon_->SetAudioClip("kettei.wav");

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

}

void HeartBottom::Draw() {
	model_->Draw(&transform_);
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
}

void HeartAbove::Update() {}

void HeartAbove::Draw() {
	model_->Draw(&transform_);
}
