#define NOMINMAX
#include "GameStartUI.h"

#include <numbers>

#include <CameraManager.h>
#include <SceneManager.h>
#include <ImGuiManager.h>

#include <WorldTime.h>
#include <Input.h>
#include <Easing.h>

#include "Heart/Heart.h"
#include "Hand/Hand.h"
#include "GameOperationUI/GameOperationUI.h"
#include "Background/Background.h"


void GameStartUI::Initialize() {
	start_.reset(new Sprite);
	start_->Initialize("start", "start.png");

	end_.reset(new Sprite);
	end_->Initialize("end", "end.png");

	for(auto& arrow : arrows_) {
		arrow.reset(new Sprite);
		arrow->Initialize("arrow", "arrow.png");
	}

	padUI_.reset(new Sprite());
	padUI_->Initialize("juuji", "juuji.png");

	padButton_.reset(new Sprite());
	padButton_->Initialize("title_sousa", "title_sousa.png");

	offset_ = { 640.0f, 500.0f, 0.0f };
	distance_ = { 0.0f, 40.0f, 0.0f };
	spriteSize_ = { 128.0f, 32.0f };

	arrowSpriteSize_ = { 20.0f, 20.0f };
	arrowOffset_ = 180.0f;
	arrows_[1]->SetAngle(1.0f * std::numbers::pi_v<float>);

	spriteColor_ = { 0,0,0,0 };

	currentSelectMode_ = 1; /// start で初期化


	/// 効果音 読み込み
	sentaku_ = new AudioSource();
	sentaku_->SetAudioClip("sentaku.wav");

	kettei_ = new AudioSource();
	kettei_->SetAudioClip("kettei.wav");



}

void GameStartUI::Update() {

	bool isUp = false;
	isUp |= Input::TriggerKey(KeyCode::UpArrow);
	isUp |= Input::TriggerPadButton(PadCode::Up);
	isUp |= Input::IsLStickStartMoving() && Vec2::Dot(Input::GetLStick(), Vec2::kUp) > 0.0f;

	bool isDown = false;
	isDown |= Input::TriggerKey(KeyCode::DownArrow);
	isDown |= Input::TriggerPadButton(PadCode::Down);
	isDown |= Input::IsLStickStartMoving() && Vec2::Dot(Input::GetLStick(), Vec2::kDown) > 0.0f;


	/// 選択しているモードを変える
	if(isDown) {
		currentSelectMode_ = std::max(currentSelectMode_ - 1, 0);
		sentaku_->PlayAudio();
	} else if(isUp) {
		currentSelectMode_ = std::min(currentSelectMode_ + 1, 1);
		sentaku_->PlayAudio();
	}


	/// ////////////////////////////////////////////////////////////////////////
	/// ゲームシーンに遷移する || ゲームを終わる
	/// ////////////////////////////////////////////////////////////////////////
	if(Input::TriggerKey(KeyCode::Enter)
	   || Input::TriggerKey(KeyCode::Space)
	   || Input::TriggerPadButton(PadCode::A)) {

		/// どちらも選択されていなければ
		if(!isGameEnded_ && !isGameStart_) {
			kettei_->PlayAudio();

			if(currentSelectMode_) { /// ゲームスタートを選択した
				EffectStartInitialize();
			} else { /// ゲーム終了を選択した
				GameEndedEffectInitialize();
			}
		}
	}

	/// 文字の色をイージングする
	animationTime_ += WorldTime::DeltaTime();
	if(animationTime_ > 5.0f) {
		spriteColor_ = Vec4(1, 1, 1, 1) * (std::cos(animationTime_ * animationSpeed_) * 0.25f + 0.75f);
	} else {
		spriteColor_ = Vec4::Lerp({ 0,0,0,0 }, { 1,1,1,1 }, animationTime_ / 5.0f);
	}
	spriteColor_.w = 1.0f;

	
	EffectStartedUpdate();		/// ゲームスタート後の更新処理
	GameEndedEffectUpdate();	/// ゲーム終了後の更新処理


	SettingSprites();
	RecalculateArrowPosition();

}

void GameStartUI::FrontSpriteDraw() {
	start_->Draw();
	end_->Draw();
	for(auto& arrow : arrows_) {
		arrow->Draw();
	}
	padUI_->Draw();
	padButton_->Draw();
}

void GameStartUI::Debug() {
	if(ImGui::TreeNodeEx("sprite", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("offset", &offset_.x, 1.0f);
		ImGui::DragFloat2("distance", &distance_.x, 1.0f);

		ImGui::DragFloat2("size", &spriteSize_.x, 1.0f);
		ImGui::ColorEdit4("color", &spriteColor_.x);

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("arrow", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("offset", &arrowOffset_, 1.0f);
		ImGui::DragFloat2("size", &arrowSpriteSize_.x, 1.0f);

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("time", &animationTime_, 0.0f);
		ImGui::DragFloat("speed", &animationSpeed_, 0.1f);

		ImGui::TreePop();
	}

}

void GameStartUI::SettingSprites() {
	/// 値のセッティング
	start_->SetPos(offset_ - distance_);
	end_->SetPos(offset_ + distance_);
	padUI_->SetPos(padUIOffset_);
	padButton_->SetPos(padButtonOffset_);

	start_->SetSize(spriteSize_);
	end_->SetSize(spriteSize_);
	padUI_->SetSize(padUISize_);
	padButton_->SetSize(padUISize_);

	start_->SetColor(spriteColor_);
	end_->SetColor(spriteColor_);
	padUI_->SetColor(spriteColor_);
	padButton_->SetColor(spriteColor_);

	for(auto& arrow : arrows_) {
		arrow->SetSize(arrowSpriteSize_);
		arrow->SetColor(spriteColor_);
	}


}

void GameStartUI::RecalculateArrowPosition() {
	Vec3 position = {};
	if(currentSelectMode_) { /// start mode
		position = offset_ - distance_;
	} else { /// end mode
		position = offset_ + distance_;
	}

	position.x += arrowOffset_;
	arrows_[0]->SetPos(position);
	position.x -= arrowOffset_ * 2.0f;
	arrows_[1]->SetPos(position);
}

void GameStartUI::EffectStartInitialize() {
	if(!isGameStart_) {
		isGameStart_ = true;

		pHeart_ = new Heart();
		pHeart_->Initialize();
		pHeart_->SetPosition({ -7.8f, -0.8f, -4.1f });
		pHeart_->SetRotate({ 0.0f, -1.0f, 0.45f });
		pHeart_->SetScale(Vec3::kOne * 0.7f);
		pHeart_->UpdateMatrix();

		pHand_ = new Hand();
		pHand_->Initialize();
		pHand_->SetPosition({ -8.05f, -0.95f, -3.9f });
		pHand_->SetRotate({ 0.0f, -0.5f, 0.0f });
		pHand_->SetScale(Vec3::kOne * 0.5f);
		pHand_->UpdateMatrix();

		pGameOperationUI_ = new GameOperationUI();
		pGameOperationUI_->Initialize();
		pGameOperationUI_->SetPosition({ -12.0f, 0.6f, 1.0f });
		pGameOperationUI_->UpdateMatrix();
		pGameOperationUI_->isActive = false;
		pGameOperationUI_->SetIsFrontBinder(false);

		auto monitorLayer = SceneManager::GetInstance()->GetSceneLayer(0);
		monitorLayer->SetIsApplyBloom(false, FRONT_SPRITE);

		auto gameLayer = SceneManager::GetInstance()->GetSceneLayer(1);
		gameLayer->SetIsApplyBloom(false, OBJECT3D);

	}
}

void GameStartUI::EffectStartedUpdate() {

	/// スタート後の演出
	if(!isGameStart_) { return; }


	startAnimationTime_ += WorldTime::DeltaTime();

	/// uiの色を変える
	spriteColor_.w = std::lerp(
		1.0f, 0.0f,
		std::min(startAnimationTime_, 1.0f)
	);


	if(startAnimationTime_ - 1.0f >= 0.0f) {

		/// カメラを遷移する
		if(!isCameraMoving_) {
			isCameraMoving_ = true;
			/// カメラの座標を計算
			BaseCamera* camera =
				CameraManager::GetInstance()->GetCamera("GameCamera");
			camera->SetMove(
				{ {0.0f, 0.2f, -15.0f}, { 0.0f, 0.0f, 0.0f } },
				{ {0.0f, 0.2f, -15.0f}, { 0.0f, -0.12f, 0.0f } },
				0.5f
			);
		}

		/// 心臓と手を左から定位置に移動させる
		float lerpT = Ease::Out::Expo(std::min((startAnimationTime_ - 1.0f) / 1.0f, 1.0f));

		if(pHeart_) {
			pHeart_->SetPosition(Vec3::Lerp(
				{ -7.8f, -0.8f, -4.1f }, { -3.8f, -0.8f, -4.1f },
				lerpT
			));
		}

		if(pHand_) {
			pHand_->SetPosition(Vec3::Lerp(
				{ -8.05f, -0.8f, -4.1f }, { -4.05f, -0.95f, -3.9f },
				lerpT
			));
		}

		if(pGameOperationUI_) {
			pGameOperationUI_->SetPosition(Vec3::Lerp(
				{ -12.0f, 0.6f, 1.0f }, { -6.0f, 0.6f, 1.0f },
				lerpT
			));
			pGameOperationUI_->UpdateMatrix();
		}

		if(lerpT == 1.0f) {
			SceneManager::GetInstance()->SetNextScene(GAME);
		}

	}

}



void GameStartUI::GameEndedEffectInitialize() {
	if(!isGameEnded_) {
		isGameEnded_ = true;


	}

}

void GameStartUI::GameEndedEffectUpdate() {
	if(!isGameEnded_) {
		return;
	}



}
