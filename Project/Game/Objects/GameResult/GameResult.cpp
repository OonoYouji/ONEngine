#include "GameResult.h"

/// std
#include <algorithm>

/// engine
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "BackTitleUI/BackTitleUI.h"
#include "RetryUI/RetryUI.h"
#include "SelectedFrame/SelectedFrame.h"

#include "Objects/BossAnimation/BossAnimation.h"
#include "Objects/TitleObjects/TitlePlayerAnimator.h"
#include "Objects/SceneTransition/SceneTransition.h"

/// scene
#include "Scenes/Scene_Game.h"


GameResult::GameResult() {
	CreateTag(this);
}

GameResult::~GameResult() {}

void GameResult::Initialize() {
	retryUI_       = new RetryUI();
	backTitleUI_   = new BackTitleUI();
	selectedFrame_ = new SelectedFrame();
	sceneTransition_ = new SceneTransition(TRANSTION_FADE_IN);

	retryUI_->Initialize();
	backTitleUI_->Initialize();
	selectedFrame_->Initialize();
	sceneTransition_->Initialize();
	sceneTransition_->drawLayerId = 2;

	/*##########################################################
		TODO : COMMENT
		ゲームのリザルトをstatic変数で宣言、取得する
		結果をここか、リザルトシーンで受け取る
	##########################################################*/

	gameResult_ = Scene_Game::sGameResult_;
	Transform* createObjectTransform = nullptr;
	TitlePlayerAnimator* playerAnimation = nullptr;
	BossAnimation*       bossAnimation   = nullptr;


	/// 結果によって生成する物を変更する
	switch(gameResult_) {
	case GAME_RESULT_CLEAR:
		playerAnimation = new TitlePlayerAnimator();
		playerAnimation->SetUseShadow(false);
		playerAnimation->Initialize();

		playerAnimation->SetBasePosition({ 0.0f, 0.0f, 0.0f });
		playerAnimation->SetParticleUseRotate(false);
		playerAnimation->GetParticleSystem()->SetEmittedParticleCount(4u);

		createObjectTransform = playerAnimation->GetTransform();
		break;
	case GAME_RESULT_GAME_OVER:
		bossAnimation = new BossAnimation();
		bossAnimation->Initialize();
		bossAnimation->SetAnimationIndex(BOSS_ANIMATION_STANDBY);

		createObjectTransform = bossAnimation->GetTransform();
		break;
	}

	createObjectTransform->position = Vec3(0, 0, 0);
}

void GameResult::Update() {
	bool isLeftInput = false;
	bool isRightInput = false;

	/*##########################################################
		TODO : COMMENT
		コントローラー操作を追加する
	##########################################################*/

	/// 左を選択する
	isLeftInput |= Input::TriggerKey(KeyCode::A);
	isLeftInput |= Input::TriggerKey(KeyCode::LeftArrow);

	/// 右を選択する
	isRightInput |= Input::TriggerKey(KeyCode::D);
	isRightInput |= Input::TriggerKey(KeyCode::RightArrow);


	/// 両方のボタンが入力されていたら無視
	if(!(isLeftInput && isRightInput)) {

		/// セレクトモードを変える
		selectedNextMode_ = selectedNextMode_ + (isRightInput - isLeftInput);
		selectedNextMode_ = std::clamp(
			selectedNextMode_,
			static_cast<int>(NEXT_MODE_RETRY),
			static_cast<int>(NEXT_MODE_BACK_TITLE)
		);


		/// 実際の位置に移動させる
		selectedFrame_->SetPositionX(
			-1.0f + (selectedNextMode_ * 2.0f)
		);

	}


	/// シーンの遷移
	bool isSceneLoad = false;
	isSceneLoad |= Input::TriggerKey(KeyCode::Space);
	isSceneLoad |= Input::TriggerPadButton(PadCode::A);

	if(isSceneLoad) {
		sceneTransition_->SetIsStarted(true);
	}

	/// 演出が終わったので遷移する
	if(sceneTransition_->GetIsFinished()) {

		if(selectedNextMode_ == NEXT_MODE_RETRY) {
			SceneManager::GetInstance()->SetNextScene(SCENE_ID::BOSS_ENTRY);
		} else {
			SceneManager::GetInstance()->SetNextScene(SCENE_ID::TITLE);
		}
	}

}
