#define NOMINMAX
#include "GameTitle.h"

#include <ImGuiManager.h>
#include <CameraManager.h>

#include <WorldTime.h>
#include <Input.h>
#include <Easing.h>

#include <GameStartUI/GameStartUI.h>
#include <Wave/Wave.h>



void GameTitle::Initialize() {
	title_.reset(new Sprite);
	title_->Initialize("title", "title.png");

	titlePosition_ = { 640, 250, 0 };
	titleSize_ = { 640, 360 };
	titleColor_ = { 0,0,0,1 };

	title_->SetPos(titlePosition_);
	title_->SetSize(titleSize_);
	title_->SetColor(titleColor_);

	startBegin_ = new AudioSource();
	startBegin_->SetAudioClip("sosei_begin.wav");

	startConstant_ = new AudioSource();
	startConstant_->SetAudioClip("sosei_constant.wav");

}

void GameTitle::Update() {


	GameStart();

	/// せっかち用のショートカット
	if(Input::TriggerKey(KeyCode::Enter)
	   || Input::TriggerKey(KeyCode::Space)
	   || Input::TriggerPadButton(PadCode::A)) {
		SettingCameraAnimation();
		CreateStartUI();
	}

	const float kMaxTime = 2.0f;

	animationTime_ += WorldTime::DeltaTime();
	if(animationTime_ > kMaxTime) {
		SettingCameraAnimation();
	}

	if(animationTime_ <= kMaxTime) {

		////////////////////////////////////////
		///		開始時の効果音再生 
		////////////////////////////////////////
		if(!isPlayingAudio_) {
			isPlayingAudio_ = true;
			startBegin_->PlayAudio();
			//startBGM_->PlayAudio();
		}

		titleColor_ = Vec4::Lerp(
			{ 0, 0, 0, 1 },
			{ 0.5f, 0.5f, 0.5f, 1 },
			Ease::InOut::Expo(animationTime_ / kMaxTime)
		);

	} else {
		titleColor_ = { 0.5f, 0.5f, 0.5f, 1.0f };

		CreateStartUI();
	}

	title_->SetColor(titleColor_);

	if(isPlayingAudio_ && !isGameStart_) {
		if(!startBegin_->IsPlayingAudio() && !startConstant_->IsPlayingAudio()) {
			startConstant_->isLoop = true;
			startConstant_->PlayAudio();
		}
	}


}

void GameTitle::FrontSpriteDraw() {
	title_->Draw();
}

void GameTitle::Debug() {
	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("time", &animationTime_, 1.0f);
		ImGui::DragFloat("speed", &animationSpeed_, 1.0f);
		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("title", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat2("position", &titlePosition_.x, 1.0f);
		ImGui::DragFloat2("size", &titleSize_.x, 1.0f);
		ImGui::ColorEdit4("color", &titleColor_.x);
		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("startEffect", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("currentTime", &startEffectAnimationTime_, 0.1f);
		ImGui::DragFloat("maxTiem", &startEffectMaxAnimationTime_, 0.1f);

		ImGui::TreePop();
	}




}



void GameTitle::SettingCameraAnimation() {
	if(!isAnimationStarted_) {
		isAnimationStarted_ = true;

		BaseCamera* camera =
			CameraManager::GetInstance()->GetCamera("GameCamera");
		camera->SetMove(
			{ {0.0f, 0.2f, -7.6f}, { 0.0f, 0.0f, 0.0f } },
			{ {0.0f, 0.2f, -15.0f}, { 0.0f, 0.0f, 0.0f } },
			0.5f
		);
	}
}

void GameTitle::CreateStartUI() {
	if(!isCreatedStartUI_) {
		isCreatedStartUI_ = true;

		pGameStartUI_ = new GameStartUI();
		pGameStartUI_->Initialize();

	}
}

void GameTitle::GameStart() {
	/// game start ui を生成していたら
	if(!pGameStartUI_) { return; }


	if(pGameStartUI_->GetIsGameStart()) {
		isGameStart_ = true;

		/// 再生中の音を止める
		startBegin_->StopAudioAll();
		startConstant_->StopAudioAll();



	}

	if(isGameStart_) {

		startEffectAnimationTime_ += WorldTime::DeltaTime();
		float lerpT = std::min(startEffectAnimationTime_ / startEffectMaxAnimationTime_, 1.0f);

		Vec2 size = {
			titleSize_.x,
			std::lerp(titleSize_.y, 0.0f, Ease::InOut::Expo(lerpT))
		};

		title_->SetSize(size);


		if(lerpT == 1.0f) {
			if(!isCreartedWave_) {
				isCreartedWave_ = true;
				/// waveの生成
				(new Wave)->Initialize();
			}
		}


	}
}
