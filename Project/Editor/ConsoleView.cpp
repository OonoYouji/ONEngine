#include "ConsoleView.h"
#include "Engine/Common/Console.h"
#include "EditorUtils.h"
#include <algorithm>

namespace Engine::Editor {

void ConsoleView::Render(bool* p_open) {
    if (p_open && !*p_open) return;
    ImGui::Begin("Console", p_open);

    // ツールバー
    if (ImGui::Button("Clear")) {
        // Engine::Console 側には現状 Clear がないため、
        // ログシステムのバッファをリセットする仕組みが必要な場合は別途実装が必要
    }
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &autoScroll_);
    ImGui::SameLine();
    
    // フィルター
    ImGui::SetNextItemWidth(150);
    ImGui::InputTextWithHint("##Filter", "Filter...", filterBuffer_, sizeof(filterBuffer_));
    ImGui::SameLine();

    // ログレベルフィルター
    ImGui::Checkbox("Info", &showInfo_); ImGui::SameLine();
    ImGui::Checkbox("Warn", &showWarning_); ImGui::SameLine();
    ImGui::Checkbox("Error", &showError_);

    ImGui::Separator();

    // ログ表示エリア
    const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar)) {
        
        const auto& logs = Engine::Console::GetLogVector();
        std::string filter(filterBuffer_);
        std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);

        for (const auto& msg : logs) {
            // フィルタリング
            bool isError = msg.find("[error]") != std::string::npos;
            bool isWarning = msg.find("[warning]") != std::string::npos;
            bool isInfo = msg.find("[info]") != std::string::npos || (!isError && !isWarning);

            if (isError && !showError_) continue;
            if (isWarning && !showWarning_) continue;
            if (isInfo && !showInfo_) continue;

            if (!filter.empty()) {
                std::string msgLower = msg;
                std::transform(msgLower.begin(), msgLower.end(), msgLower.begin(), ::tolower);
                if (msgLower.find(filter) == std::string::npos) continue;
            }

            // 色分け
            ImVec4 color = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
            if (isError) color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            else if (isWarning) color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(msg.c_str());
            ImGui::PopStyleColor();
        }

        if (autoScroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();

    EditorUtils::DrawActiveViewOutline();
    ImGui::End();
}

} // namespace Engine::Editor
