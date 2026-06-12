#include "ConsoleWindow.h"

/// std
#include <format>
#include <vector>

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/Core/Utility/Tools/Log.h"

using namespace Editor;

void ConsoleWindow::ShowImGui() {
	if (!ImGui::Begin("Console")) {
		ImGui::End();
		return;
	}

	// --- ツールバーの描画 ---
	if (ImGui::Button("Clear")) {
		ONEngine::Console::ClearLogBuffer();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Auto-scroll", &autoScroll_);
	
	ImGui::SameLine();
	ImGui::TextUnformatted("|");
	
	// レベルフィルタ
	ImGui::SameLine();
	ImGui::Checkbox("Info", &showInfo_);
	ImGui::SameLine();
	ImGui::Checkbox("Warning", &showWarning_);
	ImGui::SameLine();
	ImGui::Checkbox("Error", &showError_);

	ImGui::SameLine();
	ImGui::TextUnformatted("|");

	// カテゴリフィルタ
	ImGui::SameLine();
	ImGui::Checkbox("Engine", &showEngine_);
	ImGui::SameLine();
	ImGui::Checkbox("Script", &showScriptEngine_);
	ImGui::SameLine();
	ImGui::Checkbox("Game", &showApplication_);

	ImGui::Separator();

	// --- ログ表示領域 ---
	const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);

	const auto& allLogs = ONEngine::Console::GetLogVector();
	
	// 表示対象のログを事前にフィルタリング
	static std::vector<size_t> displayIndices;
	displayIndices.clear();
	displayIndices.reserve(allLogs.size());

	for (size_t i = 0; i < allLogs.size(); ++i) {
		const auto& entry = allLogs[i];
		
		// レベルフィルタリング
		if (entry.level == ONEngine::LogLevel::Info && !showInfo_) continue;
		if (entry.level == ONEngine::LogLevel::Warning && !showWarning_) continue;
		if (entry.level == ONEngine::LogLevel::Error && !showError_) continue;

		// カテゴリフィルタリング
		if (entry.category == ONEngine::LogCategory::Engine && !showEngine_) continue;
		if (entry.category == ONEngine::LogCategory::ScriptEngine && !showScriptEngine_) continue;
		if (entry.category == ONEngine::LogCategory::Application && !showApplication_) continue;

		displayIndices.push_back(i);
	}

	ImGuiListClipper clipper;
	clipper.Begin(static_cast<int>(displayIndices.size()));
	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			const auto& entry = allLogs[displayIndices[i]];

			// カテゴリプレフィックスと色
			const char* categoryTag = "[Unknown] ";
			if (entry.category == ONEngine::LogCategory::Engine) categoryTag = "[Engine] ";
			else if (entry.category == ONEngine::LogCategory::ScriptEngine) categoryTag = "[Script] ";
			else if (entry.category == ONEngine::LogCategory::Application) categoryTag = "[Game]   ";
			
			// レベルに応じた色分け
			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Default White
			if (entry.level == ONEngine::LogLevel::Warning) color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
			if (entry.level == ONEngine::LogLevel::Error) color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);   // Light Red

			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::Text("%s%s", categoryTag, entry.message.c_str());
			ImGui::PopStyleColor();
		}
	}

	if (autoScroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();

	ImGui::Separator();

	// --- 下部のステータス表示 ---
	std::string&& stats = std::format("fps: {:.2f} | delta: {:.4f}s", 1.0f / ONEngine::Time::DeltaTime(), ONEngine::Time::DeltaTime());
	ImGui::Text(stats.c_str());

	ImGui::End();
}
