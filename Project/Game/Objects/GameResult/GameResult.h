#pragma once

#include "GameObjectManager/GameObjectManager.h"


enum SELECTED_NEXT_MOVE {
	NEXT_MODE_RETRY,      /// リトライを選択している
	NEXT_MODE_BACK_TITLE, /// タイトルへ戻るを選択している
};

enum GAME_RESULT {
	GAME_RESULT_CLEAR,	  /// ゲームクリア
	GAME_RESULT_GAME_OVER /// ゲームオーバー
};


class GameResult : public BaseGameObject {
public:

	GameResult();
	~GameResult();

	void Initialize() override;
	void Update()     override;

private:

	class RetryUI*       retryUI_       = nullptr;
	class BackTitleUI*   backTitleUI_   = nullptr;
	class SelectedFrame* selectedFrame_ = nullptr;

	/// enum variables
	int selectedNextMode_ = 0;
	int gameResult_       = 0;
};