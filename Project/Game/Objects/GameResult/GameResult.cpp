#include "GameResult.h"

/// std
#include <algorithm>

/// engine
#include "Input/Input.h"

/// objects
#include "BackTitleUI/BackTitleUI.h"
#include "RetryUI/RetryUI.h"
#include "SelectedFrame/SelectedFrame.h"


GameResult::GameResult() {
	CreateTag(this);
}

GameResult::~GameResult() {}

void GameResult::Initialize() {
	retryUI_       = new RetryUI();
	backTitleUI_   = new BackTitleUI();
	selectedFrame_ = new SelectedFrame();

	retryUI_->Initialize();
	backTitleUI_->Initialize();
	selectedFrame_->Initialize();

}

void GameResult::Update() {
	bool isLeftInput  = false;
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
		selectedNextMode_ = selectedNextMode_ + (isRightInput - isLeftInput);
		selectedNextMode_ = std::clamp(
			selectedNextMode_, 
			static_cast<int>(NEXT_MODE_RETRY),
			static_cast<int>(NEXT_MODE_BACK_TITLE)
		);

		selectedFrame_->SetPositionX(
			-1.0f + (selectedNextMode_ * 2.0f)
		);

	}


}
