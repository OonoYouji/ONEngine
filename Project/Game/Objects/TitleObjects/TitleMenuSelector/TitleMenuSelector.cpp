#include "TitleMenuSelector.h"

/// std
#include <algorithm>

/// engine
#include "Input/Input.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// objects
#include "../TitleStartText/TitleStartText.h"
#include "../TitleExitText/TitleExitText.h"

/// scenes
#include "Scenes/Scene_Title.h"


TitleMenuSelector::TitleMenuSelector() {
	CreateTag(this);
}

TitleMenuSelector::~TitleMenuSelector() {}

void TitleMenuSelector::Initialize() {
	drawLayerId = TITLE_LAYER_UI;

	selectMenuIndex_ = 0;
	isSelected_      = false;

	startText_ = new TitleStartText();
	startText_->Initialize();

	exitText_ = new TitleExitText();
	exitText_->Initialize();

	texts_[SELECT_MENU_START] = startText_;
	texts_[SELECT_MENU_EXIT] = exitText_;


	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("Arrow.png");

}

void TitleMenuSelector::Update() {

	/// menuを選択する
	UpdateMenu();
	/// 決めたmenuを始める
	UpdateSelect();
}



void TitleMenuSelector::UpdateMenu() {
	bool isUp = false;
	bool isDown = false;

	isUp |= Input::TriggerKey(KeyCode::W);
	isUp |= Input::TriggerKey(KeyCode::ArrowUp);
	isUp |= Input::TriggerPadButton(PadCode::Up);
	isUp |= Input::IsLStickStartMoving() && Input::GetLeftStick().y > 0.0f;

	isDown |= Input::TriggerKey(KeyCode::S);
	isDown |= Input::TriggerKey(KeyCode::ArrowDown);
	isDown |= Input::TriggerPadButton(PadCode::Down);
	isDown |= Input::IsLStickStartMoving() && Input::GetLeftStick().y > 0.0f;


	if(isUp) { --selectMenuIndex_; }
	if(isDown) { ++selectMenuIndex_; }


	selectMenuIndex_ = std::clamp(selectMenuIndex_, 0, 1);
	pTransform_->position = texts_[selectMenuIndex_]->GetPosition();
	pTransform_->rotate = texts_[selectMenuIndex_]->GetRotate();
	pTransform_->scale = texts_[selectMenuIndex_]->GetScale();
}


void TitleMenuSelector::UpdateSelect() {
	bool isExe = false;

	isExe |= Input::TriggerKey(KeyCode::Space);
	isExe |= Input::TriggerPadButton(PadCode::A);


	/// 選択したmenuを開始する
	if(isExe) {
		isSelected_ = true;
	}

}

