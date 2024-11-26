#include "UIManager.h"

#include "InputUI/InputUI.h"


UIManager::UIManager() {
	CreateTag(this);
}

UIManager::~UIManager() {}

void UIManager::Initialize() {

	uis_.push_back(new InputUI());

	for(auto& ui : uis_) {
		ui->Initialize();
	}

}

void UIManager::Update() {

	for(auto& ui : uis_) {
		ui->drawLayerId = drawLayerId;
	}

}

