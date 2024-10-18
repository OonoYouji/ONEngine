#pragma once

#include "GameObjectManager/GameObjectManager.h"


enum SELECTED_NEXT_MOVE {
	NEXT_MODE_RETRY,      /// リトライを選択している
	NEXT_MODE_BACK_TITLE, /// タイトルへ戻るを選択している
};



class GameResult : public BaseGameObject {
public:

	GameResult();
	~GameResult();

	void Initialize() override;
	void Update()     override;

private:

	class RetryUI*     retryUI_     = nullptr;
	class BackTitleUI* backTitleUI_ = nullptr;

	int selectedNextMode_ = 0;
};
