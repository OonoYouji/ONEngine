#include "ConsoleManager.h"



void ConsoleManager::Initialize() {
	parentImGuiWindowFlags_ = 0;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;
	parentImGuiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;

	imguiWindowFlags_ = 0;
	imguiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	imguiWindowFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
}

void ConsoleManager::Finalize() {}

void ConsoleManager::Update() {

	ParentWindow();

	for(auto& func : debugFunctions_) {
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		func(imguiWindowFlags_);
	}
}



void ConsoleManager::ParentWindow() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Parent Window", nullptr, parentImGuiWindowFlags_)) {
		ImGui::End();
		return;
	}


	/// ===================================================
	/// main window menu baer
	/// ===================================================
	if(ImGui::BeginMenuBar()) {

		for(auto& func : menuFunctions_) {
			func();
		}

		ImGui::EndMenuBar();
	}

	ImGui::DockSpace(ImGui::GetID("DockSpace"));

	if(parentImGuiWindowFlags_ & ImGuiWindowFlags_NoMove) {
		ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	}

	if(parentImGuiWindowFlags_ & ImGuiWindowFlags_NoResize) {
		ImGui::SetWindowSize(ImVec2(1280.0f, 720.0f));
	}


	ImGui::End();
}

void ConsoleManager::RegisterFunction(DebugFunction _function) {
	debugFunctions_.push_back(_function);
}
