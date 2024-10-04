#include "Console.h"


#include <GameObjectManager.h>


void Console::Initialize() {

	parentWinFlags_ = 0;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;
	parentWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar;

	imguiWinFlags_ = 0;
	imguiWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	//imguiWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
	//imguiWinFlags_ |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
	imguiWinIsOpen_ = true;
}

void Console::Update() {
#ifdef _DEBUG
	ParentWindow();

	Inspector();
	Herarchy();

	Scene();
	Debug();

	Assets();
#endif // _DEBUG
}



void Console::ParentWindow() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	if(!ImGui::Begin("Parent Window", nullptr, parentWinFlags_)) {
		ImGui::End();
		return;
	}


	if(ImGui::BeginMenuBar()) {

		if(ImGui::BeginMenu("setting")) {

			bool noResize = parentWinFlags_ & ImGuiWindowFlags_NoResize;
			if(ImGui::Checkbox("NoResize", &noResize)) {
				if(noResize) {
					parentWinFlags_ |= ImGuiWindowFlags_NoResize;
				} else {
					parentWinFlags_ &= ~ImGuiWindowFlags_NoResize;
				}
			}

			bool noMove = parentWinFlags_ & ImGuiWindowFlags_NoMove;
			if(ImGui::Checkbox("NoMove", &noMove)) {
				if(noMove) {
					parentWinFlags_ |= ImGuiWindowFlags_NoMove;
				} else {
					parentWinFlags_ &= ~ImGuiWindowFlags_NoMove;
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::DockSpace(ImGui::GetID("DockSpace"));

	if(parentWinFlags_ & ImGuiWindowFlags_NoMove) {
		ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	}

	if(parentWinFlags_ & ImGuiWindowFlags_NoResize) {
		ImGui::SetWindowSize(ImVec2(1280.0f, 720.0f));
	}


	ImGui::End();
}

void Console::Inspector() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	if(!ImGui::Begin("Inspector", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}




	ImGui::End();
}

void Console::Herarchy() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Heararchy", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}

void Console::Scene() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Scene", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}

void Console::Debug() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Debug", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}

void Console::Assets() {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if(!ImGui::Begin("Assets", nullptr, imguiWinFlags_)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}
