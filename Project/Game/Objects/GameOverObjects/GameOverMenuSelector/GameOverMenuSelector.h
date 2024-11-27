#pragma once

#include "GameObjectManager/BaseGameObject.h"


enum SELECT_MENU {
	SELECT_MENU_RESTART, 
	SELECT_MENU_TITLE
};



class GameOverMenuSelector : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameOverMenuSelector();
	~GameOverMenuSelector();

	void Initialize() override;
	void Update()     override;

	void AddVariables();
	void ApplyVariables();

	//bool GetIsSelected() const { return isSelected_; }
	int GetSelectMenu() const { return selectMenuIndex_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* renderer_ = nullptr;

	BaseGameObject* texts_[2];
	int selectMenuIndex_;

};
