#define NOMINMAX
#include "GameClearEffect.h"

#include <ImGuiManager.h>
#include <CameraManager.h>
#include <SceneManager.h>

#include <WorldTime/WorldTime.h>
#include <Easing.h>
#include <Input.h>

#include "Wave/Wave.h"
#include "Hand/Hand.h"



void GameClearEffect::Initialize() {


	/// 手が前に来て手をふる
	hand_ = new Hand;
	hand_->Initialize();
	hand_->SetPosition({ -4.05f, -0.95f, -3.9f });
	hand_->SetRotate({ 0.0f, -0.5f, 0.0f });
	hand_->SetScale(Vec3::kOne * 0.5f);
	hand_->UpdateMatrix();
	hand_->isActive = false;


	/// 2, 波が落ち着く
	wave_ = new Wave();
	wave_->Initialize();
	wave_->SetAmplitude(110.0f);

	/// 3, 感謝状を出す

	letter_.reset(new Sprite);
	letter_->Initialize("letter", "letter.png");

	letterPosition_ = Vec3(640.0f, 360.0f, 0.0f);
	letterSize_ = Vec2(640.0f, 360.0f);
	letter_->SetPos(letterPosition_);
	letter_->SetSize(letterSize_);

}



void GameClearEffect::Update() {

	switch(currentPhase_) {
	case HAND_MOVING:

		HandMoving();
		StoppingWave();
		break;
	case CAMERA_MOVING:

		CameraMoving();
		break;
	case STOPPING_WAVE:

		StoppingWave();
		break;
	case DRAW_THANK_YOU_LETTER:

		DrawThankYouLetter();
		ShakeHand();
		break;
	case EFFECT_END:

		EffectEnd();
		ShakeHand();
		break;
	case WAIT:

		waitTime_ -= WorldTime::DeltaTime();
		if(waitTime_ <= 0.0f) {
			currentPhase_ = nextPhase_;
			waitTime_ = 0.0f;
		}
		break;
	}

	hand_->UpdateMatrix();


}

void GameClearEffect::Draw() {}

void GameClearEffect::FrontSpriteDraw() {

	if(isDrawLetter_) {
		letter_->Draw();
	}

}

void GameClearEffect::Debug() {
	if(ImGui::TreeNodeEx("letter", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("position", &letterPosition_.x);
		ImGui::DragFloat2("size", &letterSize_.x);

		ImGui::DragFloat("current time", &letterAnimationTime_);
		ImGui::DragFloat("max time", &letterMaxAnimationTime_);

		letter_->SetSize(letterSize_);

		ImGui::TreePop();
	}
}


void GameClearEffect::HandMoving() {

	handAnimationTime_ += WorldTime::DeltaTime();
	float lerpT = std::min(handAnimationTime_ / (handMaxAnimationTime_ / 2.0f), 2.0f);

	if(lerpT <= 1.0f) {

		hand_->SetPosition(Vec3::Lerp(
			{ -4.05f, -0.95f, -3.9f }, { -4.05f, 1.15f, -3.9f },
			lerpT
		));

	} else {

		hand_->SetPosition(Vec3::Lerp(
			{ -4.05f, 1.15f, -3.9f }, { -3.35f, -0.7f, -6.7f },
			lerpT - 1.0f
		));
	}

	hand_->UpdateMatrix();

	if(lerpT == 2.0f) {
		SetNextPhase(DRAW_THANK_YOU_LETTER, 0.0f);
		wave_->isDrawActive = false;
	}

}

void GameClearEffect::CameraMoving() {
	if(cameraMovingTime_ == 0.0f) {
		BaseCamera* camera =
			CameraManager::GetInstance()->GetCamera("GameCamera");
		camera->SetMove(
			{ { 0.0f, 0.2f, -15.0f }, { 0.0f, -0.12f, 0.0f } },
			{ { 0.0f, 0.3f, -7.6f }, { 0.0f, 0.0f, 0.0f } },
			cameraMovingMaxTime_
		);
	}

	cameraMovingTime_ += WorldTime::DeltaTime();

	/// 移動の終了
	if(cameraMovingTime_ / cameraMovingMaxTime_ >= 1.0f) {
		SceneManager::GetInstance()->SetNextScene(TITLE);
	}

}

void GameClearEffect::StoppingWave() {

	waveAnimationTime_ += WorldTime::DeltaTime();

	float lerpT = std::min(waveAnimationTime_ / waveMaxAnimationTime_, 1.0f);

	float alpha = std::lerp(
		1.0f, 0.0f,
		Ease::In::Expo(lerpT)
	);

	float amplitude = std::lerp(
		110.0f, 0.0f,
		Ease::Out::Expo(lerpT)
	);

	wave_->SetAlpha(alpha);
	wave_->SetAmplitude(amplitude);

	///次のフェーズに行く
	if(lerpT == 2.0f) {
		//SetNextPhase(DRAW_THANK_YOU_LETTER, 0.0f);
		wave_->isDrawActive = false;
	}

}

void GameClearEffect::DrawThankYouLetter() {

	isDrawLetter_ = true;

	letterAnimationTime_ += WorldTime::DeltaTime();
	float lerpT = std::min(letterAnimationTime_ / letterMaxAnimationTime_, 1.0f);

	float alpha = std::lerp(0.0f, 1.0f, (lerpT));

	letter_->SetPos(letterPosition_);
	letter_->SetSize(letterSize_);
	letter_->SetColor({ 1,1,1, alpha });

	if(lerpT == 1.0f) {
		SetNextPhase(EFFECT_END, 0.0f);
	}

}

void GameClearEffect::EffectEnd() {
	bool inputTrigger = false;
	inputTrigger |= Input::TriggerKey(KeyCode::Space);
	inputTrigger |= Input::TriggerKey(KeyCode::Enter);
	inputTrigger |= Input::TriggerPadButton(PadCode::A);

	if(inputTrigger) {
		//SceneManager::GetInstance()->SetNextScene(TITLE);
		SetNextPhase(CAMERA_MOVING, 0.0f);
	}

}



void GameClearEffect::SetNextPhase(EFFECT_PHASE next, float waitTime) {
	currentPhase_ = WAIT;
	nextPhase_ = next;
	waitTime_ = waitTime;
}



void GameClearEffect::ShakeHand() {

	shakeHandAnimationTime_ += WorldTime::DeltaTime();

	float sinValue = std::sin(8.0f * shakeHandAnimationTime_) * 0.5f + 0.5f;

	hand_->SetRotate(Vec3(
		0.0f,
		-0.5f * sinValue,
		0.5f * sinValue
	));



}
