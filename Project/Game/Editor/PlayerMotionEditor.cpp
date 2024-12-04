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


	addMotionName_ = "none";
	addMotionValue_ = 0;

}

void PlayerMotionEditor::Finalize() {}

void PlayerMotionEditor::ImGuiDebug(ImGuiWindowFlags _windowFlags) {
	ImGui::Begin("PlayerMotionEditor", nullptr, _windowFlags);

	AddMotionEdit();

	RenderMotionAll();

	ImGui::End();
}

void PlayerMotionEditor::AddMotionEdit() {
	ImGui::SeparatorText("add motion edit");

	ImGui::Spacing();

	ImGui::InputText("motion name", addMotionName_.data(), 256);
	ImGui::DragInt("motion value", &addMotionValue_);

	ImGui::Spacing();

	if(ImGui::Button("add")) {
		AddMotion(addMotionName_, addMotionValue_);
	}
}

void PlayerMotionEditor::AddMotion(const std::string& _motionName, int _value) {
	if(_motionName[0] == '\0') {
		MessageBoxA(nullptr, "motion name empty!!!", "Failed", MB_OK | MB_ICONERROR);
		return;
	}

	motions_[_motionName] = _value;

}

void PlayerMotionEditor::RenderMotionAll() {
	ImGui::SeparatorText("render motion all");
	if(ImGui::TreeNodeEx("motions")) {
		for(auto& motion : motions_) {
			ImGui::Text(motion.first.c_str());
		}
		ImGui::TreePop();
	}
}
