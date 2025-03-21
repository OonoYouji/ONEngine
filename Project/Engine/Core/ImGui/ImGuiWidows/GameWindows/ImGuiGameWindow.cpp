#include "ImGuiGameWindow.h"

/// external
#include <imgui.h>

/// engine
#include "../GameWindows/ImGuiProjectWindow.h"
#include "../GameWindows/ImGuiGameSceneWindow.h"
#include "../GameWindows/ImGuiSceneWindow.h"
#include "../GameWindows/ImGuiInspectorWindow.h"
#include "../GameWindows/ImGuiHierarchyWindow.h"

ImGuiGameWindow::ImGuiGameWindow(EntityCollection* _entityCollection, GraphicsResourceCollection* _resourceCollection) {

	/// windowの設定
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	/// ここでwindowを生成する
	AddChild(std::make_unique<ImGuiProjectWindow>());
	AddChild(std::make_unique<ImGuiGameSceneWindow>());
	AddChild(std::make_unique<ImGuiSceneWindow>(_resourceCollection));
	AddChild(std::make_unique<ImGuiInspectorWindow>());
	AddChild(std::make_unique<ImGuiHierarchyWindow>(_entityCollection));
}



void ImGuiGameWindow::ImGuiFunc() {

	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(1280, 700), ImGuiCond_Appearing);
	if (!ImGui::Begin("Game", nullptr, imGuiFlags_)) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID("GameDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	///// ウィンドウの描画
	//ImGui::SetWindowPos("Game", ImVec2(0, 20), ImGuiCond_Always);
	//ImGui::SetWindowSize("Game", ImVec2(1280, 700), ImGuiCond_Always);
	//ImGui::SetWindowFocus(nullptr);


	UpdateChildren();

	ImGui::End();
}