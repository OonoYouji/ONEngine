#pragma once

#include "GameObjectManager/BaseGameObject.h"

enum SELECT_MENU {
	SELECT_MENU_START,
	SELECT_MENU_EXIT,
};


class TitleMenuSelector : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TitleMenuSelector();
	~TitleMenuSelector();

	void Initialize() override;
	void Update()     override;


	void UpdateMenu();
	void UpdateSelect();

	bool GetIsSelected() const { return isSelected_; }
	int GetSelectMenu() const { return selectMenuIndex_; }


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	int selectMenuIndex_; /// 選択しているMenu
	bool isSelected_;

	/// component
	class SpriteRenderer* spriteRenderer_ = nullptr;
	
	/// other class
	class TitleStartText* startText_ = nullptr;
	class TitleExitText*  exitText_  = nullptr;
	
	BaseGameObject* texts_[2];

};
