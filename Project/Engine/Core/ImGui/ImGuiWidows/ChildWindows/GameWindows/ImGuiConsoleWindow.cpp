#include "ImGuiConsoleWindow.h"

/// std
#include <format>

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/Core/Utility/Tools/Log.h"

void ImGuiConsoleWindow::ShowImGui() {

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

		logCounts_.clear();
		for (const auto& message : Console::GetLogVector()) {
			if (indices_.contains(message)) {
				logCounts_[message]++;
			} else {
				indices_[message] = logs_.size();
				logs_.push_back(message);
				logCounts_[message]++;
			}
		}

		for (const auto& log : logs_) {
			ImGui::Text("count: %d", logCounts_[log]);
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::Text(log.c_str());
		}

		ImGui::EndPopup();
	}

	if (ImGui::IsItemClicked(1)) {
		ImGui::OpenPopup("LogPopup");
	}

	ImGui::End();
}
