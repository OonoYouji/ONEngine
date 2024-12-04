#include "PlayerMotionEditor.h"

/// engine
#include "WindowManager/ConsoleManager.h"


PlayerMotionEditor::PlayerMotionEditor(Player* _player) 
	: pPlayer_(_player) {
}

PlayerMotionEditor::~PlayerMotionEditor() {}

void PlayerMotionEditor::Initialize() {
	ConsoleManager::GetInstance()->RegisterFunction(
		[this](ImGuiWindowFlags _windowFlags) { ImGuiDebug(_windowFlags); }
	);

}

void PlayerMotionEditor::Finalize() {}

void PlayerMotionEditor::ImGuiDebug(ImGuiWindowFlags _windowFlags) {
	ImGui::Begin("PlayerMotionEditor", nullptr, _windowFlags);


	ImGui::End();
}
