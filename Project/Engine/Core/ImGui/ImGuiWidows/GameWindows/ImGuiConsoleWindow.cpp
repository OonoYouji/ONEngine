#include "ImGuiConsoleWindow.h"

/// std
#include <format>

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/Core/Utility/Tools/Log.h"

void ImGuiConsoleWindow::ImGuiFunc() {

	if (!ImGui::Begin("Console")) {
		ImGui::End();
		return;
	}

	std::string&& text = std::format("fps: {:.3f} / delta time: {:.3f}", 1.0f / Time::DeltaTime(), Time::DeltaTime());
	ImGui::Text(text.c_str());

	ImGui::SameLine();
	ImGui::Text(" : ");
	ImGui::SameLine();

	// 現在のログを取得し、整形して表示
	std::string currentLog = Console::GetCurrentLog();
	ImGui::Text(currentLog.c_str());

	// 右クリックで全てのログを表示するポップアップを開く
	if (ImGui::BeginPopupContextItem("LogPopup")) {
		ImGui::Text("All Logs:");
		ImGui::Separator();
		ImGui::Text(Console::GetAllLogs().c_str());
		ImGui::EndPopup();
	}

	if (ImGui::IsItemClicked(1)) {
		ImGui::OpenPopup("LogPopup");
	}

	ImGui::End();
}
