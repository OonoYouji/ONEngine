#include "BehaviorTreeEditorWindow.h"
#include <imgui.h>
#include <format>
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Core/Utility/Utility.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <set>

using json = nlohmann::json;

namespace Editor {

BehaviorTreeEditorWindow* BehaviorTreeEditorWindow::s_Instance = nullptr;

BehaviorTreeEditorWindow::BehaviorTreeEditorWindow(const std::string& title, ONEngine::EntityComponentSystem* ecs)
    : pEcs_(ecs), m_WindowTitle(title) {
    s_Instance = this;
    ed::Config config;
    config.SettingsFile = nullptr; // 自動保存によるハングアップの原因を特定するため再度無効化
    m_Editor = ed::CreateEditor(&config);

    m_LastAutoSaveTime = ONEngine::Time::GetTime();

    // InitializeEditor() は ShowImGui の初回呼び出しまで遅延させる
}

BehaviorTreeEditorWindow::~BehaviorTreeEditorWindow() {
    if (s_Instance == this) s_Instance = nullptr;
    if (m_Editor) {
        ed::DestroyEditor(m_Editor);
    }
}

void BehaviorTreeEditorWindow::InitializeEditor() {
    static bool isInitializing = false;
    if (isInitializing) return;
    isInitializing = true;

    try {
        ONEngine::Console::Log("BTEditor: Starting simplified class scan...");
        availableNodeClasses_ = ONEngine::MonoScriptEngine::GetInstance().GetBehaviorNodeClasses();
        // availableModuleClasses_ の検索はハングの疑いがあるため一旦スキップ
        RefreshFileList();
        ONEngine::Console::Log("BTEditor: Simplified class scan completed.");
        m_EditorInitialized = true;
    } catch (...) {
        ONEngine::Console::LogError("BTEditor: Fatal error during class scan!");
    }
    
    isInitializing = false;
}

void BehaviorTreeEditorWindow::RefreshFileList() {
    m_AvailableTrees.clear();
    std::string basePath = "Assets/AITrees";
    if (std::filesystem::exists(basePath)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(basePath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                // basePath からの相対パスを取得して保存
                std::string fullPath = entry.path().string();
                std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
                m_AvailableTrees.push_back(fullPath);
            }
        }
    }
}

void BehaviorTreeEditorWindow::DrawFileBrowser() {
    if (ImGui::Button("Refresh Files")) RefreshFileList();
    ImGui::Separator();

    ImGui::BeginChild("FileList");
    for (const auto& filePath : m_AvailableTrees) {
        bool isSelected = (m_CurrentFilePath == filePath);
        // 表示用にはファイル名だけ、または短いパスを表示
        std::string displayName = filePath;
        if (filePath.find("Assets/AITrees/") == 0) {
            displayName = filePath.substr(15); // "Assets/AITrees/" を削る
        }

        if (ImGui::Selectable(displayName.c_str(), isSelected)) {
            m_CurrentFilePath = filePath;
            LoadTree(m_CurrentFilePath);
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to Load: %s", filePath.c_str());
    }
    ImGui::EndChild();
}

void BehaviorTreeEditorWindow::ShowImGui() {
    s_Instance = this;

    if (!m_EditorInitialized) {
        InitializeEditor();
    }

    // オートセーブ処理
    float currentTime = ONEngine::Time::GetTime();
    if (currentTime - m_LastAutoSaveTime > m_AutoSaveInterval) {
        m_LastAutoSaveTime = currentTime;
        if (!m_CurrentFilePath.empty()) {
            std::string backupPath = m_CurrentFilePath + ".bak";
            SaveTree(backupPath);
        }
    }

    if (!ONEngine::DebugConfig::isDebugging) {
        m_RuntimeNodeStatuses.clear();
        m_RuntimeBBValues.clear();
        m_NodeLastActiveTime.clear();
    }

    // ウィンドウタイトルに '*' を表示（IDを固定するために ### を使用）
    std::string displayTitle = m_WindowTitle + (m_IsDirty ? "*" : "") + "###" + m_WindowTitle;
    if (!ImGui::Begin(displayTitle.c_str(), nullptr)) {
        ImGui::End();
        return;
    }

    // ショートカットキー
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z)) Undo();
        if (ImGui::GetIO().KeyCtrl && (ImGui::IsKeyPressed(ImGuiKey_Y) || (ImGui::GetIO().KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z)))) Redo();
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) CopySelectedNodes();
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V)) PasteNodes();
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D)) DuplicateSelectedNodes();

        // ズーム・フォーカス操作
        if (!ImGui::IsAnyItemActive()) {
            if (ImGui::IsKeyPressed(ImGuiKey_F)) ed::NavigateToSelection(true);
            if (ImGui::IsKeyPressed(ImGuiKey_A)) ed::NavigateToContent(true);
        }
    }

    // ツールバー
    if (ImGui::Button("Undo")) Undo(); ImGui::SameLine();
    if (ImGui::Button("Redo")) Redo(); ImGui::SameLine();
    ImGui::TextDisabled("|"); ImGui::SameLine();
if (ImGui::Button("Save")) { if (!m_CurrentFilePath.empty()) SaveTree(m_CurrentFilePath); }
ImGui::SameLine();
if (ImGui::Button("Save As...")) { ImGui::OpenPopup("Save As"); }
ImGui::SameLine();
if (ImGui::Button("New")) { RecordUndo(); m_Nodes.clear(); m_Links.clear(); m_BBVariables.clear(); m_CurrentFilePath = ""; CreateNode("Entry"); ed::NavigateToContent(); }
ImGui::SameLine();
if (ImGui::Button("Focus")) { ed::SetCurrentEditor(m_Editor); ed::NavigateToContent(); ed::SetCurrentEditor(nullptr); }

// --- エラー一覧の表示 ---
std::vector<std::string> allErrors;
for (const auto& node : m_Nodes) {
    if (node.hasError) {
        allErrors.push_back(std::format("Node '{}' (ID:{}): {}", node.name, (uintptr_t)node.id, node.validationError));
    }
}

if (!allErrors.empty()) {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "  (!) %llu Errors Detected", allErrors.size());
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        for (const auto& err : allErrors) ImGui::Text("- %s", err.c_str());
        ImGui::EndTooltip();
    }
}

ImGui::Columns(2);
    if (ImGui::Button("Refresh")) { InitializeEditor(); RefreshFileList(); }
    ImGui::SameLine();
    if (ONEngine::DebugConfig::isPause) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("RESUME")) ONEngine::DebugConfig::isPause = false;
        ImGui::PopStyleColor(2);
    } else {
        if (ImGui::Button("PAUSE")) ONEngine::DebugConfig::isPause = true;
    }
    ImGui::SameLine();
    ImGui::Text("Nodes:%d Modules:%d", (int)availableNodeClasses_.size(), (int)availableModuleClasses_.size());

    // Save As Popup
    if (ImGui::BeginPopupModal("Save As", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char nameBuf[64] = "NewTree";
        ImGui::Text("Enter new file name:");
        ImGui::InputText(".json", nameBuf, sizeof(nameBuf));
        
        std::string newPath = "Assets/AITrees/" + std::string(nameBuf) + ".json";
        bool exists = std::filesystem::exists(newPath);
        if (exists) {
            ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Warning: File already exists and will be overwritten.");
        }

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            SaveTree(newPath);
            m_CurrentFilePath = newPath;
            RefreshFileList();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::Text("Current File: %s", m_CurrentFilePath.empty() ? "(Unsaved)" : m_CurrentFilePath.c_str());

    ImGui::Separator();

    ImGuiStyle& style = ImGui::GetStyle();
    float spacing = style.ItemSpacing.x;
    float leftWidth = 220.0f;
    float rightWidth = 450.0f; 
    float centerWidth = ImGui::GetContentRegionAvail().x - leftWidth - rightWidth - (spacing * 2.0f);

    if (ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), true)) {
        if (ImGui::BeginTabBar("LeftTabs")) {
            if (ImGui::BeginTabItem("Files")) { DrawFileBrowser(); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("Blackboard")) { DrawBlackboardEditor(); ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();
    if (ImGui::BeginChild("CenterPanel", ImVec2(centerWidth, 0), true)) DrawGraphEditor();
    ImGui::EndChild();
    ImGui::SameLine();
    if (ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), true)) DrawNodeInspector();
    ImGui::EndChild();

    ImGui::End();
}

void BehaviorTreeEditorWindow::DrawBlackboardEditor() {
    if (ImGui::BeginTabBar("BlackboardTabs")) {
        if (ImGui::BeginTabItem("Edit")) {
            if (ImGui::Button("Add Variable")) { RecordUndo(); m_BBVariables.push_back({ "NewVar", BBVarType::Float }); }
            ImGui::BeginChild("VariablesListEdit");
            for (size_t i = 0; i < m_BBVariables.size(); ++i) {
                auto& var = m_BBVariables[i];
                ImGui::PushID(static_cast<int>(i));
                if (ImGui::Button("X")) { RecordUndo(); m_BBVariables.erase(m_BBVariables.begin() + i); ImGui::PopID(); break; }
                ImGui::SameLine();
                char keyBuf[64]; strncpy_s(keyBuf, var.key.c_str(), sizeof(keyBuf));
                ImGui::PushItemWidth(100);
                if (ImGui::InputText("##Key", keyBuf, sizeof(keyBuf))) { var.key = keyBuf; }
                if (ImGui::IsItemActivated()) RecordUndo();
                ImGui::PopItemWidth();
                ImGui::SameLine();
                const char* typeNames[] = { "Int", "Float", "Bool", "Vec3", "String" };
                int typeIdx = (int)var.type;
                ImGui::PushItemWidth(70);
                if (ImGui::Combo("##Type", &typeIdx, typeNames, IM_ARRAYSIZE(typeNames))) { RecordUndo(); var.type = static_cast<BBVarType>(typeIdx); }
                ImGui::PopItemWidth();
                ImGui::Indent(20.0f);
                switch (var.type) {
                case BBVarType::Int: if (ImGui::InputInt("Value", &var.iVal)) {} if (ImGui::IsItemActivated()) RecordUndo(); break;
                case BBVarType::Float: if (ImGui::InputFloat("Value", &var.fVal)) {} if (ImGui::IsItemActivated()) RecordUndo(); break;
                case BBVarType::Bool: if (ImGui::Checkbox("Value", &var.bVal)) RecordUndo(); break;
                case BBVarType::Vector3: if (ImGui::DragFloat3("Value", var.vVal, 0.1f)) {} if (ImGui::IsItemActivated()) RecordUndo(); break;
                case BBVarType::String: if (ImGui::InputText("Value", var.sVal, sizeof(var.sVal))) {} if (ImGui::IsItemActivated()) RecordUndo(); break;
                }
                ImGui::Unindent(20.0f);
                ImGui::Separator();
                ImGui::PopID();
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Runtime")) {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Live Watcher");
            ImGui::Separator();
            ImGui::BeginChild("VariablesListRuntime");
            for (auto it = m_RuntimeBBValues.begin(); it != m_RuntimeBBValues.end(); ++it) {
                uint32_t keyHash = it->first;
                const auto& runtimeVal = it->second;
                std::string keyName = "Unknown";
                for (const auto& var : m_BBVariables) {
                    uint32_t hash = 2166136261;
                    for (char c : var.key) hash = (hash ^ c) * 16777619;
                    if (hash == keyHash) { keyName = var.key; break; }
                }
                ImGui::Text("%s:", keyName.c_str()); ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", runtimeVal.value.c_str());
                ImGui::TextDisabled("  Type: %s", runtimeVal.typeName.c_str());
                ImGui::Separator();
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

static int s_SelectedModuleId = -1;
static bool s_SelectedModuleIsService = false;
static int s_SelectedCommentId = -1;

void BehaviorTreeEditorWindow::DrawNodeInspector() {
    ImGui::Text("Inspector");
    ImGui::Separator();

    ImGui::PushItemWidth(-100.0f);

    if (s_SelectedCommentId != -1) {
        if (s_SelectedCommentId >= (int)m_CommentBoxes.size()) { s_SelectedCommentId = -1; ImGui::PopItemWidth(); return; }
        auto& cb = m_CommentBoxes[s_SelectedCommentId];
        ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Comment Box");
        ImGui::Separator();
        char buf[256]; strncpy_s(buf, cb.text.c_str(), sizeof(buf));
        if (ImGui::InputTextMultiline("Text", buf, sizeof(buf), ImVec2(0, 60))) cb.text = buf;
        if (ImGui::IsItemActivated()) RecordUndo();
        float col[4]; ImVec4 cv = cb.color; col[0] = cv.x; col[1] = cv.y; col[2] = cv.z; col[3] = cv.w;
        if (ImGui::ColorEdit4("Color", col)) cb.color = ImColor(col[0], col[1], col[2], col[3]);
        if (ImGui::IsItemActivated()) RecordUndo();
        if (ImGui::Button("Remove Comment Box")) { RecordUndo(); m_CommentBoxes.erase(m_CommentBoxes.begin() + s_SelectedCommentId); s_SelectedCommentId = -1; }
        ImGui::PopItemWidth();
        return;
    }

    Node* selectedNode = nullptr;
    if (m_SelectedNodeId) { for (auto& node : m_Nodes) { if (node.id == m_SelectedNodeId) { selectedNode = &node; break; } } }
    if (!selectedNode) { ImGui::TextDisabled("Select a node."); ImGui::PopItemWidth(); return; }

    ImGui::TextColored(ImVec4(1,1,0,1), "%s", selectedNode->name.c_str());
    ImGui::Separator();

    char nameBuf[64]; strncpy_s(nameBuf, selectedNode->name.c_str(), sizeof(nameBuf));
    if (ImGui::InputText("Node Name", nameBuf, sizeof(nameBuf))) selectedNode->name = nameBuf;
    if (ImGui::IsItemActivated()) RecordUndo();
    
    // ブレークポイント表示
    if (selectedNode->hasBreakpoint) ImGui::TextColored(ImVec4(1, 0, 0, 1), "BREAKPOINT ACTIVE");
    if (selectedNode->hasError) {
        ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "VALIDATION ERROR:");
        ImGui::TextDisabled("%s", selectedNode->validationError.c_str());
    }

    if (ImGui::Button(selectedNode->hasBreakpoint ? "Disable Breakpoint" : "Enable Breakpoint")) {
        RecordUndo();
        selectedNode->hasBreakpoint = !selectedNode->hasBreakpoint;
    }

    if (ImGui::CollapsingHeader("Modules", ImGuiTreeNodeFlags_DefaultOpen)) {
        for (int i = 0; i < (int)selectedNode->decorators.size(); ++i) {
            auto& d = selectedNode->decorators[i];
            bool isSelected = (s_SelectedModuleId == i && !s_SelectedModuleIsService);
            if (d.hasError) { ImGui::TextColored(ImVec4(1, 0, 0, 1), "!"); ImGui::SameLine(); }
            if (ImGui::Selectable(std::format("[D] {}", d.name).c_str(), isSelected)) { s_SelectedModuleId = i; s_SelectedModuleIsService = false; }
            if (d.hasError && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", d.validationError.c_str());
        }
        for (int i = 0; i < (int)selectedNode->services.size(); ++i) {
            auto& s = selectedNode->services[i];
            bool isSelected = (s_SelectedModuleId == i && s_SelectedModuleIsService);
            if (s.hasError) { ImGui::TextColored(ImVec4(1, 0, 0, 1), "!"); ImGui::SameLine(); }
            if (ImGui::Selectable(std::format("[S] {}", s.name).c_str(), isSelected)) { s_SelectedModuleId = i; s_SelectedModuleIsService = true; }
            if (s.hasError && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", s.validationError.c_str());
        }
    }
    ImGui::Separator();

    std::string targetClassName;
    std::map<std::string, std::string>* targetProps = nullptr;
    if (s_SelectedModuleId == -1) { targetClassName = selectedNode->className; targetProps = &selectedNode->properties; }
    else {
        auto& modules = s_SelectedModuleIsService ? selectedNode->services : selectedNode->decorators;
        if (s_SelectedModuleId < (int)modules.size()) {
            auto& m = modules[s_SelectedModuleId];
            if (m.hasError) {
                ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "MODULE ERROR:");
                ImGui::TextDisabled("%s", m.validationError.c_str());
            }
            targetClassName = m.className; targetProps = &m.properties;
            if (ImGui::Button("Move Up") && s_SelectedModuleId > 0) { RecordUndo(); std::swap(modules[s_SelectedModuleId], modules[s_SelectedModuleId - 1]); s_SelectedModuleId--; }
            ImGui::SameLine();
            if (ImGui::Button("Move Down") && s_SelectedModuleId < (int)modules.size() - 1) { RecordUndo(); std::swap(modules[s_SelectedModuleId], modules[s_SelectedModuleId + 1]); s_SelectedModuleId++; }
            ImGui::SameLine();
            if (ImGui::Button("Remove Module")) { RecordUndo(); modules.erase(modules.begin() + s_SelectedModuleId); s_SelectedModuleId = -1; ImGui::PopItemWidth(); return; }
        }
    }

    if (targetProps && !targetClassName.empty() && targetClassName != "Entry") {
        auto fields = ONEngine::MonoScriptEngine::GetInstance().GetClassFields(targetClassName);
        for (const auto& field : fields) {
            ImGui::PushID(field.name.c_str());
            std::string currentVal = (*targetProps)[field.name];
            if (field.isBBKey) {
                std::vector<const char*> bbKeys; int currentIdx = -1;
                for (int i = 0; i < (int)m_BBVariables.size(); ++i) { bbKeys.push_back(m_BBVariables[i].key.c_str()); if (m_BBVariables[i].key == currentVal) currentIdx = i; }
                if (ImGui::Combo(field.name.c_str(), &currentIdx, bbKeys.data(), (int)bbKeys.size())) { RecordUndo(); (*targetProps)[field.name] = bbKeys[currentIdx]; }
            } else if (field.typeName == "System.Single" || field.typeName == "float") {
                float f = currentVal.empty() ? 0.0f : std::stof(currentVal);
                if (ImGui::DragFloat(field.name.c_str(), &f, 0.1f)) { (*targetProps)[field.name] = std::to_string(f); }
                if (ImGui::IsItemActivated()) RecordUndo();
            } else if (field.typeName == "System.Int32" || field.typeName == "int") {
                int i = currentVal.empty() ? 0 : std::stoi(currentVal);
                if (ImGui::InputInt(field.name.c_str(), &i)) { (*targetProps)[field.name] = std::to_string(i); }
                if (ImGui::IsItemActivated()) RecordUndo();
            } else if (field.typeName == "System.Boolean" || field.typeName == "bool") {
                bool b = (currentVal == "true");
                if (ImGui::Checkbox(field.name.c_str(), &b)) { RecordUndo(); (*targetProps)[field.name] = b ? "true" : "false"; }
            } else if (field.typeName == "System.String" || field.typeName == "string") {
                char sBuf[128]; strncpy_s(sBuf, currentVal.c_str(), sizeof(sBuf));
                if (ImGui::InputText(field.name.c_str(), sBuf, sizeof(sBuf))) { (*targetProps)[field.name] = sBuf; }
                if (ImGui::IsItemActivated()) RecordUndo();
            } else if (field.typeName.find("Vector4") != std::string::npos || field.typeName.find("Color") != std::string::npos) {
                // Vector4 または Color と名のつくものは ColorEdit4 で編集できるようにする
                float col[4] = { 1, 1, 1, 1 };
                if (!currentVal.empty()) {
                    std::stringstream ss(currentVal);
                    std::string segment;
                    int i = 0;
                    while (std::getline(ss, segment, ',') && i < 4) {
                        try { col[i++] = std::stof(segment); } catch (...) {}
                    }
                }
                if (ImGui::ColorEdit4(field.name.c_str(), col)) {
                    (*targetProps)[field.name] = std::format("{},{},{},{}", col[0], col[1], col[2], col[3]);
                }
                if (ImGui::IsItemActivated()) RecordUndo();
            } else {
                // 未知の型（Enum等）は現状文字列として編集可能にする
                char sBuf[128]; strncpy_s(sBuf, currentVal.c_str(), sizeof(sBuf));
                if (ImGui::InputText(std::format("{} ({})", field.name, field.typeName).c_str(), sBuf, sizeof(sBuf))) { (*targetProps)[field.name] = sBuf; }
                if (ImGui::IsItemActivated()) RecordUndo();
            }
            ImGui::PopID();
        }
    }
    ImGui::PopItemWidth();
}

void BehaviorTreeEditorWindow::DrawGraphEditor() {
    ValidateNodes();
    ed::SetCurrentEditor(m_Editor);
    ed::Begin("BT_Graph_Editor", ImVec2(0, 0));

    float currentTime = ONEngine::Time::GetTime();

    std::map<uintptr_t, int> executionOrder;
    int currentOrder = 1;
    auto entryIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [](const Node& n) { return n.className == "Entry"; });
    if (entryIt != m_Nodes.end()) {
        std::deque<Node*> stack; stack.push_back(&(*entryIt));
        std::set<uintptr_t> visited;
        int safetyCounter = 0;
        const int MAX_NODES = 1000;

        while (!stack.empty() && safetyCounter++ < MAX_NODES) {
            Node* current = stack.back(); stack.pop_back();
            uintptr_t ptr = (uintptr_t)current->id.AsPointer();
            
            if (visited.count(ptr)) continue;
            visited.insert(ptr);

            if (current->className != "Entry") executionOrder[ptr] = currentOrder++;
            
            // 現在のノードからの出力を探す
            std::vector<Node*> children;
            for (const auto& link : m_Links) {
                bool isOutputOfCurrent = false;
                for (const auto& pin : current->outputs) {
                    if (pin.id == link.startPinId) { isOutputOfCurrent = true; break; }
                }
                
                if (isOutputOfCurrent) {
                    for (auto& targetNode : m_Nodes) {
                        bool found = false;
                        for (const auto& inPin : targetNode.inputs) {
                            if (inPin.id == link.endPinId) {
                                // 循環参照を防止：既に訪問済みのノードは追加しない
                                if (visited.find((uintptr_t)targetNode.id.AsPointer()) == visited.end()) {
                                    children.push_back(&targetNode);
                                }
                                found = true;
                                break;
                            }
                        }
                        if (found) break;
                    }
                }
            }
            // Y座標でソートして実行順を決定（上にあるものが先）
            std::sort(children.begin(), children.end(), [](Node* a, Node* b) { 
                return ed::GetNodePosition(a->id).y > ed::GetNodePosition(b->id).y; 
            });
            for (auto* child : children) stack.push_back(child);
        }
    }

    for (int i = 0; i < (int)m_CommentBoxes.size(); ++i) {
        auto& cb = m_CommentBoxes[i];
        ed::PushStyleColor(ed::StyleColor_NodeBg, cb.color);
        ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(20, 20, 20, 20));
        ed::BeginNode(ed::NodeId(cb.id));
        ImGui::PushID(cb.id);
        ImGui::TextColored(ImVec4(1, 1, 1, 0.8f), "%s", cb.text.c_str());
        ImGui::Dummy(cb.size);
        if (ed::GetSelectedObjectCount() > 0 && ed::IsNodeSelected(ed::NodeId(cb.id))) { s_SelectedCommentId = i; m_SelectedNodeId = 0; }
        ImGui::PopID();
        ed::EndNode();
        ed::PopStyleVar();
        ed::PopStyleColor();
    }

    if (ed::GetSelectedObjectCount() > 0) {
        ed::NodeId selectedNodes[1];
        if (ed::GetSelectedNodes(selectedNodes, 1) > 0) {
            bool isComment = false;
            for (const auto& cb : m_CommentBoxes) if (cb.id == (uint32_t)(uintptr_t)selectedNodes[0].AsPointer()) { isComment = true; break; }
            if (!isComment && m_SelectedNodeId != selectedNodes[0]) { m_SelectedNodeId = selectedNodes[0]; s_SelectedModuleId = -1; s_SelectedCommentId = -1; }
        }
    } else { if (!ImGui::IsAnyItemActive()) { m_SelectedNodeId = 0; s_SelectedModuleId = -1; s_SelectedCommentId = -1; } }

    std::map<uintptr_t, float> nodeAlpha;

    for (auto& node : m_Nodes) {
        uint32_t idHash = node.runtimeId; // FIX: 内部エディタIDではなく、JSON上のID（C#側との同期用）を使用する
        
        float lastTime = m_NodeLastActiveTime.count(idHash) ? m_NodeLastActiveTime[idHash] : -100.0f;
        float elapsed = currentTime - lastTime;
        float alpha = (std::max)(0.0f, 1.0f - (elapsed / 2.0f)); 
        nodeAlpha[(uintptr_t)node.id.AsPointer()] = alpha;

        bool isHighlighted = false; ImColor highlightColor = ImColor(255, 255, 0, 0);
        if (m_RuntimeNodeStatuses.count(idHash)) {
            int status = m_RuntimeNodeStatuses[idHash];
            if (status >= 0 && status <= 2) {
                isHighlighted = true;
                switch (status) {
                case 0: highlightColor = ImColor(0, 255, 0); break;   // Success: Green
                case 1: highlightColor = ImColor(255, 0, 0); break;   // Failure: Red
                case 2: highlightColor = ImColor(255, 255, 0); break; // Running: Yellow
                }
            }
        }
        
        if (node.hasBreakpoint) { ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 0, 0)); ed::PushStyleVar(ed::StyleVar_NodeBorderWidth, 6.0f); }
        else if (isHighlighted) { ed::PushStyleColor(ed::StyleColor_NodeBorder, highlightColor); ed::PushStyleVar(ed::StyleVar_NodeBorderWidth, 4.0f); }
        else if (alpha > 0.01f) {
            ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(1.0f, 1.0f, 1.0f, alpha * 0.5f));
            ed::PushStyleVar(ed::StyleVar_NodeBorderWidth, 2.0f + alpha * 2.0f);
        }

        ed::BeginNode(node.id);
        ImGui::PushID(node.id.AsPointer());
        
        if (node.hasBreakpoint) { ImGui::TextColored(ImVec4(1, 0, 0, 1), "(BP)"); ImGui::SameLine(); }

        ImVec4 titleColor = node.isDecorator ? ImVec4(0.3f, 0.6f, 1.0f, 1.0f) : ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
        ImGui::TextColored(titleColor, "== %s ==", node.name.c_str());
        if (node.hasError) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "[!] ");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", node.validationError.c_str());
        }

        if (executionOrder.count((uintptr_t)node.id.AsPointer())) { ImGui::SameLine(120); ImGui::TextDisabled("[%d]", executionOrder[(uintptr_t)node.id.AsPointer()]); }
        for (const auto& d : node.decorators) {
            if (d.hasError) { ImGui::TextColored(ImVec4(1, 0, 0, 1), "!"); ImGui::SameLine(); }
            ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "[D] %s", d.name.c_str());
            if (d.hasError && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", d.validationError.c_str());
        }
        for (const auto& s : node.services) {
            if (s.hasError) { ImGui::TextColored(ImVec4(1, 0, 0, 1), "!"); ImGui::SameLine(); }
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "[S] %s", s.name.c_str());
            if (s.hasError && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", s.validationError.c_str());
        }
        ImGui::Spacing();
        ImGui::BeginGroup();
        for (auto& pin : node.inputs) { ed::BeginPin(pin.id, ed::PinKind::Input); ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), ">"); ed::EndPin(); ImGui::SameLine(); ImGui::TextUnformatted(pin.name.c_str()); }
        ImGui::SameLine(100.0f);
        for (auto& pin : node.outputs) { ImGui::TextUnformatted(pin.name.c_str()); ImGui::SameLine(); ed::BeginPin(pin.id, ed::PinKind::Output); ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), ">"); ed::EndPin(); }
        ImGui::EndGroup();
        ImGui::PopID();
        ed::EndNode();
        if (node.hasBreakpoint || isHighlighted || alpha > 0.01f) { ed::PopStyleVar(); ed::PopStyleColor(); }
    }
    
    for (auto& link : m_Links) {
        float alpha = 0.0f;
        uintptr_t startNodePtr = 0, endNodeAlphaPtr = 0;
        for(const auto& n : m_Nodes) {
            for(const auto& p : n.outputs) if(p.id == link.startPinId) startNodePtr = (uintptr_t)n.id.AsPointer();
            for(const auto& p : n.inputs) if(p.id == link.endPinId) endNodeAlphaPtr = (uintptr_t)n.id.AsPointer();
        }
        if(startNodePtr && endNodeAlphaPtr) alpha = (std::min)(nodeAlpha[startNodePtr], nodeAlpha[endNodeAlphaPtr]);

        float thickness = 2.0f + alpha * 3.0f;
        ImColor col = link.color;
        if(alpha > 0.01f) col = ImColor(1.0f, 1.0f, 1.0f, 0.3f + alpha * 0.7f);
        
        ed::Link(link.id, link.startPinId, link.endPinId, col, thickness);
        if(alpha > 0.5f) ed::Flow(link.id);
    }

    if (ed::BeginCreate()) {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
            if (inputPinId && outputPinId) if (ed::AcceptNewItem()) { RecordUndo(); CreateLink(inputPinId, outputPinId); }
        }

        ed::PinId newNodePinId;
        if (ed::QueryNewNode(&newNodePinId)) {
            if (ed::AcceptNewItem()) {
                m_NewNodeLinkPinId = newNodePinId;
                m_ContextNodePos = ed::ScreenToCanvas(ImGui::GetMousePos());
                ImGui::OpenPopup("Create New Node");
            }
        }
    }
    ed::EndCreate();

    if (ed::BeginDelete()) {
        ed::LinkId linkId;
        while (ed::QueryDeletedLink(&linkId)) {
            if (ed::AcceptDeletedItem()) {
                RecordUndo();
                auto it = std::remove_if(m_Links.begin(), m_Links.end(), [linkId](const Link& l) { return l.id == linkId; });
                m_Links.erase(it, m_Links.end());
            }
        }

        ed::NodeId nodeId;
        while (ed::QueryDeletedNode(&nodeId)) {
            auto it = std::find_if(m_Nodes.begin(), m_Nodes.end(), [nodeId](const Node& n) { return n.id == nodeId; });
            if (it != m_Nodes.end()) {
                if (it->className == "Entry") {
                    ed::RejectDeletedItem();
                } else if (ed::AcceptDeletedItem()) {
                    RecordUndo();
                    m_Nodes.erase(it);
                }
                continue;
            }

            auto itCb = std::find_if(m_CommentBoxes.begin(), m_CommentBoxes.end(), [nodeId](const CommentBox& cb) {
                return cb.id == (uint32_t)(uintptr_t)nodeId.AsPointer();
            });
            if (itCb != m_CommentBoxes.end()) {
                if (ed::AcceptDeletedItem()) {
                    RecordUndo();
                    m_CommentBoxes.erase(itCb);
                }
                continue;
            }

            // 見つからないID（内部的なものなど）に対しても必ずRejectを呼ばないと無限ループになる
            ed::RejectDeletedItem();
        }
    }
    ed::EndDelete();

    ed::Suspend();
    ed::NodeId contextNodeId;
    if (ed::ShowNodeContextMenu(&contextNodeId)) { 
        bool isComment = false;
        for(int i=0; i<(int)m_CommentBoxes.size(); ++i) if(m_CommentBoxes[i].id == (uint32_t)(uintptr_t)contextNodeId.AsPointer()) { s_SelectedCommentId = i; isComment = true; break; }
        if(!isComment) { m_SelectedNodeId = contextNodeId; ImGui::OpenPopup("Node Context Menu"); }
    }
    if (ImGui::BeginPopup("Node Context Menu")) {
        if (ImGui::MenuItem("Toggle Breakpoint")) {
            RecordUndo();
            for (auto& node : m_Nodes) if (node.id == m_SelectedNodeId) { node.hasBreakpoint = !node.hasBreakpoint; break; }
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Add Decorator")) {
            static char decSearchBuf[64] = ""; if (ImGui::IsWindowAppearing()) { decSearchBuf[0] = '\0'; ImGui::SetKeyboardFocusHere(); }
            ImGui::InputTextWithHint("##DecSearch", "Search...", decSearchBuf, sizeof(decSearchBuf));
            std::string searchStr = decSearchBuf; std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);
            ImGui::Separator();
            for (const auto& m : availableModuleClasses_) if (!m.isService) {
                std::string nameLower = m.fullName; std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                if (!searchStr.empty() && nameLower.find(searchStr) == std::string::npos) continue;
                if (ImGui::MenuItem(m.fullName.c_str())) { RecordUndo(); for (auto& node : m_Nodes) if (node.id == m_SelectedNodeId) { node.decorators.push_back({ (uint32_t)GetNextId(), m.fullName, m.fullName, {}, false }); break; } }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Add Service")) {
            static char srvSearchBuf[64] = ""; if (ImGui::IsWindowAppearing()) { srvSearchBuf[0] = '\0'; ImGui::SetKeyboardFocusHere(); }
            ImGui::InputTextWithHint("##SrvSearch", "Search...", srvSearchBuf, sizeof(srvSearchBuf));
            std::string searchStr = srvSearchBuf; std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);
            ImGui::Separator();
            for (const auto& m : availableModuleClasses_) if (m.isService) {
                std::string nameLower = m.fullName; std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                if (!searchStr.empty() && nameLower.find(searchStr) == std::string::npos) continue;
                if (ImGui::MenuItem(m.fullName.c_str())) { RecordUndo(); for (auto& node : m_Nodes) if (node.id == m_SelectedNodeId) { node.services.push_back({ (uint32_t)GetNextId(), m.fullName, m.fullName, {}, true }); break; } }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    if (ed::ShowBackgroundContextMenu()) { m_ContextNodePos = ed::ScreenToCanvas(ImGui::GetMousePos()); ImGui::OpenPopup("Create New Node"); }
    if (ImGui::BeginPopup("Create New Node")) {
        static char nodeSearchBuf[64] = ""; if (ImGui::IsWindowAppearing()) { nodeSearchBuf[0] = '\0'; ImGui::SetKeyboardFocusHere(); }
        ImGui::InputTextWithHint("##NodeSearch", "Search...", nodeSearchBuf, sizeof(nodeSearchBuf));
        std::string searchStr = nodeSearchBuf; std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);
        ImGui::Separator();

        auto drawNodeItems = [&](const std::vector<ONEngine::MonoScriptEngine::NodeClassInfo>& classes, bool filterGame) {
            for (const auto& classInfo : classes) {
                // 名前空間やクラス名で Engine / Game を簡易判定
                // ※現在は名前空間が空の場合が多いため、プロジェクト特有のノード名をブラックリスト/ホワイトリスト方式で判定
                bool isGameNode = (
                    classInfo.fullName.find("MoveToPlayer") != std::string::npos ||
                    classInfo.fullName.find("InvokeEvent") != std::string::npos ||
                    classInfo.fullName.find("Sensing") != std::string::npos ||
                    classInfo.fullName.find("CheckDistance") != std::string::npos ||
                    classInfo.fullName.find("FindPlayer") != std::string::npos ||
                    classInfo.fullName.find("DefaultFocus") != std::string::npos
                );

                if (isGameNode != filterGame) continue;

                std::string nameLower = classInfo.fullName; std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                if (!searchStr.empty() && nameLower.find(searchStr) == std::string::npos) continue;
                
                if (ImGui::MenuItem(classInfo.fullName.c_str())) {
                    RecordUndo();
                    Node* node = CreateNode(classInfo.fullName, classInfo.isDecorator);
                    ed::SetNodePosition(node->id, m_ContextNodePos);
                    
                    if (m_NewNodeLinkPinId) {
                        Pin* startPin = nullptr;
                        for (auto& n : m_Nodes) {
                            for (auto& p : n.inputs) if (p.id == m_NewNodeLinkPinId) startPin = &p;
                            for (auto& p : n.outputs) if (p.id == m_NewNodeLinkPinId) startPin = &p;
                        }
                        if (startPin) {
                            if (startPin->kind == PinKind::Output && !node->inputs.empty()) CreateLink(startPin->id, node->inputs[0].id);
                            else if (startPin->kind == PinKind::Input && !node->outputs.empty()) CreateLink(node->outputs[0].id, startPin->id);
                        }
                        m_NewNodeLinkPinId = 0;
                    }
                }
            }
        };

        if (ImGui::BeginMenu("Engine Nodes")) {
            drawNodeItems(availableNodeClasses_, false);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Game Nodes")) {
            drawNodeItems(availableNodeClasses_, true);
            ImGui::EndMenu();
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Add Comment Box")) { RecordUndo(); m_CommentBoxes.push_back({ (uint32_t)GetNextId(), "New Comment", m_ContextNodePos, ImVec2(200, 200) }); }
        ImGui::EndPopup();
    }
    ed::Resume();
    ed::End();
    ed::SetCurrentEditor(nullptr);
}

BehaviorTreeEditorWindow::Node* BehaviorTreeEditorWindow::CreateNode(const std::string& className, bool isDecorator) {
    int nextId = GetNextId();
    m_Nodes.emplace_back(nextId, className);
    Node* node = &m_Nodes.back();
    node->runtimeId = (uint32_t)nextId; // Default runtimeId for new nodes
    node->className = className; node->isDecorator = isDecorator;
    if (className == "Entry") node->color = ImColor(255, 128, 0);
    else if (className.find("Sequence") != std::string::npos || className.find("Selector") != std::string::npos) node->color = ImColor(60, 60, 60);
    else if (isDecorator) node->color = ImColor(20, 60, 100); 
    else node->color = ImColor(100, 40, 100);
    if (className != "Entry") { node->inputs.emplace_back(GetNextId(), "In", PinKind::Input); node->inputs.back().node = node; }
    if (className.find("Sequence") != std::string::npos || className.find("Selector") != std::string::npos || className.find("Parallel") != std::string::npos || className == "Entry") {
        node->outputs.emplace_back(GetNextId(), "Out", PinKind::Output); node->outputs.back().node = node;
    }
    return node;
}

void BehaviorTreeEditorWindow::CreateLink(ed::PinId startPin, ed::PinId endPin) { m_Links.emplace_back(GetNextId(), startPin, endPin); }
void BehaviorTreeEditorWindow::UpdateNodeStatus(uint32_t nodeIdHash, int status, const std::string& treePath) { 
    // 現在エディタで開いているファイルパスと一致する場合のみ、実行状態を更新する
    if (treePath != m_CurrentFilePath) return;

    m_RuntimeNodeStatuses[nodeIdHash] = status; 
    
    // 実行中(Running=2) または 成功(Success=0) の場合のみ、フローを流すための時刻を更新する。
    // Failure(1) は通常フローを止めたい場合が多いが、アクティブであった証跡を残すために時刻更新は 0 と 2 に限定。
    if (status == 0 || status == 2) {
        m_NodeLastActiveTime[nodeIdHash] = ONEngine::Time::GetTime(); 
    }
    // Inactive(4) 時の時刻リセットを削除することで、2秒間のフェードアウト（残像）が有効になる。
}
void BehaviorTreeEditorWindow::UpdateBlackboardValue(uint32_t keyHash, const std::string& value, const std::string& typeName) { m_RuntimeBBValues[keyHash] = { value, typeName }; }

json BehaviorTreeEditorWindow::GetTreeAsJson() {
    ed::SetCurrentEditor(m_Editor);
    json data;
    data["blackboard"] = json::array();
    for (const auto& var : m_BBVariables) {
        json v; v["key"] = var.key; v["type"] = static_cast<int>(var.type); v["iVal"] = var.iVal; v["fVal"] = var.fVal; v["bVal"] = var.bVal; v["vVal"] = { var.vVal[0], var.vVal[1], var.vVal[2] }; v["sVal"] = var.sVal;
        data["blackboard"].push_back(v);
    }
    data["nodes"] = json::array();
    for (const auto& node : m_Nodes) {
        json n; n["id"] = node.runtimeId; n["className"] = node.className; n["name"] = node.name; n["isDecorator"] = node.isDecorator; n["hasBreakpoint"] = node.hasBreakpoint;
        n["properties"] = json::object(); for (const auto& p : node.properties) n["properties"][p.first] = p.second;
        n["decorators"] = json::array();
        for (const auto& d : node.decorators) { json dj; dj["className"] = d.className; dj["name"] = d.name; dj["properties"] = d.properties; n["decorators"].push_back(dj); }
        n["services"] = json::array();
        for (const auto& s : node.services) { json sj; sj["className"] = s.className; sj["name"] = s.name; sj["properties"] = s.properties; n["services"].push_back(sj); }
        ImVec2 pos = ed::GetNodePosition(node.id); n["pos"] = { pos.x, pos.y };
        n["inputs"] = json::array(); for (const auto& pin : node.inputs) n["inputs"].push_back({ {"id", (uintptr_t)pin.id.AsPointer()}, {"name", pin.name} });
        n["outputs"] = json::array(); for (const auto& pin : node.outputs) n["outputs"].push_back({ {"id", (uintptr_t)pin.id.AsPointer()}, {"name", pin.name} });
        data["nodes"].push_back(n);
    }
    data["links"] = json::array();
    for (const auto& link : m_Links) {
        json l; l["id"] = (uintptr_t)link.id.AsPointer(); l["startPin"] = (uintptr_t)link.startPinId.AsPointer(); l["endPin"] = (uintptr_t)link.endPinId.AsPointer();
        data["links"].push_back(l);
    }
    data["comments"] = json::array();
    for (const auto& cb : m_CommentBoxes) {
        json c; c["id"] = cb.id; c["text"] = cb.text; c["pos"] = { cb.pos.x, cb.pos.y }; c["size"] = { cb.size.x, cb.size.y };
        ImVec4 cv = cb.color; c["color"] = { cv.x, cv.y, cv.z, cv.w };
        data["comments"].push_back(c);
    }
    return data;
}

void BehaviorTreeEditorWindow::ApplyTreeFromJson(const json& data) {
    ed::SetCurrentEditor(m_Editor);
    m_Nodes.clear(); m_Links.clear(); m_BBVariables.clear(); m_CommentBoxes.clear(); m_NextId = 1;
    m_RuntimeNodeStatuses.clear(); // NEW: ファイル切り替え時に古い実行状態をクリア
    m_NodeLastActiveTime.clear();
    m_RuntimeBBValues.clear();

    if (data.contains("blackboard")) {
        for (const auto& v : data["blackboard"]) {
            BBVariable var; var.key = v["key"]; var.type = static_cast<BBVarType>(v["type"].get<int>()); var.iVal = v["iVal"]; var.fVal = v["fVal"]; var.bVal = v["bVal"];
            if (v.contains("vVal")) { var.vVal[0] = v["vVal"][0].get<float>(); var.vVal[1] = v["vVal"][1].get<float>(); var.vVal[2] = v["vVal"][2].get<float>(); }
            if (v.contains("sVal")) strncpy_s(var.sVal, v["sVal"].get<std::string>().c_str(), sizeof(var.sVal));
            m_BBVariables.push_back(var);
        }
    }
    std::map<uintptr_t, ed::PinId> pinIdMap;
    if (data.contains("nodes")) {
        for (const auto& n : data["nodes"]) {
            std::string className = n["className"]; bool isDecorator = n.value("isDecorator", false);
            Node* node = CreateNode(className, isDecorator);
            node->runtimeId = n["id"]; // FIX: JSON上のIDを同期用に保持
            node->name = n["name"];
            node->hasBreakpoint = n.value("hasBreakpoint", false);
            if (n.contains("properties")) { for (auto it = n["properties"].begin(); it != n["properties"].end(); ++it) node->properties[it.key()] = it.value(); }
            if (n.contains("decorators")) {
                for (const auto& d : n["decorators"]) node->decorators.push_back({ (uint32_t)GetNextId(), d["className"], d["name"], d["properties"].get<std::map<std::string, std::string>>(), false });
            }
            if (n.contains("services")) {
                for (const auto& s : n["services"]) node->services.push_back({ (uint32_t)GetNextId(), s["className"], s["name"], s["properties"].get<std::map<std::string, std::string>>(), true });
            }
            if (n.contains("inputs") && n["inputs"].size() == node->inputs.size()) { for (size_t i = 0; i < node->inputs.size(); ++i) pinIdMap[(uintptr_t)n["inputs"][i]["id"]] = node->inputs[i].id; }
            if (n.contains("outputs") && n["outputs"].size() == node->outputs.size()) { for (size_t i = 0; i < node->outputs.size(); ++i) pinIdMap[(uintptr_t)n["outputs"][i]["id"]] = node->outputs[i].id; }
            ed::SetNodePosition(node->id, ImVec2(n["pos"][0].get<float>(), n["pos"][1].get<float>()));
        }
    }
    if (data.contains("links")) {
        for (const auto& l : data["links"]) {
            uintptr_t startId = l["startPin"]; uintptr_t endId = l["endPin"];
            if (pinIdMap.count(startId) && pinIdMap.count(endId)) CreateLink(pinIdMap[startId], pinIdMap[endId]);
        }
    }
    if (data.contains("comments")) {
        for (const auto& c : data["comments"]) {
            CommentBox cb; cb.id = c["id"]; cb.text = c["text"]; cb.pos = ImVec2(c["pos"][0].get<float>(), c["pos"][1].get<float>()); cb.size = ImVec2(c["size"][0].get<float>(), c["size"][1].get<float>());
            cb.color = ImColor(c["color"][0].get<float>(), c["color"][1].get<float>(), c["color"][2].get<float>(), c["color"][3].get<float>());
            m_CommentBoxes.push_back(cb);
            ed::SetNodePosition(ed::NodeId(cb.id), cb.pos);
        }
    }

    // NEW: 自動的に全ノードを画面に収める
    ed::NavigateToContent(0.0f);
}

void BehaviorTreeEditorWindow::RecordUndo() {
    m_IsDirty = true;
    m_UndoStack.push_back(GetTreeAsJson());
    if (m_UndoStack.size() > m_MaxUndoSteps) m_UndoStack.pop_front();
    m_RedoStack.clear();
}

void BehaviorTreeEditorWindow::Undo() {
    if (m_UndoStack.empty()) return;
    m_RedoStack.push_back(GetTreeAsJson());
    ApplyTreeFromJson(m_UndoStack.back());
    m_UndoStack.pop_back();
}

void BehaviorTreeEditorWindow::Redo() {
    if (m_RedoStack.empty()) return;
    m_UndoStack.push_back(GetTreeAsJson());
    ApplyTreeFromJson(m_RedoStack.back());
    m_RedoStack.pop_back();
}

void BehaviorTreeEditorWindow::CopySelectedNodes() {
    ed::SetCurrentEditor(m_Editor);
    std::vector<ed::NodeId> selectedNodes;
    selectedNodes.resize(ed::GetSelectedObjectCount());
    int count = ed::GetSelectedNodes(selectedNodes.data(), (int)selectedNodes.size());
    selectedNodes.resize(count);
    if (selectedNodes.empty()) return;
    json clip;
    clip["nodes"] = json::array();
    for (auto id : selectedNodes) {
        for (const auto& node : m_Nodes) {
            if (node.id == id) {
                json n; n["className"] = node.className; n["name"] = node.name; n["isDecorator"] = node.isDecorator; n["hasBreakpoint"] = node.hasBreakpoint;
                n["properties"] = node.properties;
                n["decorators"] = json::array();
                for (const auto& d : node.decorators) { json dj; dj["className"] = d.className; dj["name"] = d.name; dj["properties"] = d.properties; n["decorators"].push_back(dj); }
                n["services"] = json::array();
                for (const auto& s : node.services) { json sj; sj["className"] = s.className; sj["name"] = s.name; sj["properties"] = s.properties; n["services"].push_back(sj); }
                ImVec2 pos = ed::GetNodePosition(node.id); n["pos"] = { pos.x, pos.y };
                clip["nodes"].push_back(n);
                break;
            }
        }
    }
    m_Clipboard = clip.dump();
}

void BehaviorTreeEditorWindow::PasteNodes() {
    if (m_Clipboard.empty()) return;
    json clip; try { clip = json::parse(m_Clipboard); } catch (...) { return; }
    RecordUndo();
    ed::SetCurrentEditor(m_Editor);
    ed::ClearSelection();
    for (const auto& n : clip["nodes"]) {
        Node* node = CreateNode(n["className"], n.value("isDecorator", false));
        node->name = n["name"];
        node->hasBreakpoint = n.value("hasBreakpoint", false);
        if (n.contains("properties")) node->properties = n["properties"].get<std::map<std::string, std::string>>();
        if (n.contains("decorators")) {
            for (const auto& d : n["decorators"]) node->decorators.push_back({ (uint32_t)GetNextId(), d["className"], d["name"], d["properties"].get<std::map<std::string, std::string>>(), false });
        }
        if (n.contains("services")) {
            for (const auto& s : n["services"]) node->services.push_back({ (uint32_t)GetNextId(), s["className"], s["name"], s["properties"].get<std::map<std::string, std::string>>(), true });
        }
        ed::SetNodePosition(node->id, ImVec2(n["pos"][0].get<float>() + 50.0f, n["pos"][1].get<float>() + 50.0f));
        ed::SelectNode(node->id, true);
    }
}

void BehaviorTreeEditorWindow::DuplicateSelectedNodes() {
    CopySelectedNodes();
    PasteNodes();
}

void BehaviorTreeEditorWindow::ValidateNodes() {
    // 全ノードのピン情報を収集
    std::set<uintptr_t> allPinIds;
    for (const auto& node : m_Nodes) {
        for (const auto& pin : node.inputs) allPinIds.insert((uintptr_t)pin.id);
        for (const auto& pin : node.outputs) allPinIds.insert((uintptr_t)pin.id);
    }

    for (auto& node : m_Nodes) {
        node.hasError = false;
        node.validationError = "";

        // --- 1. 接続の有無チェック ---
        bool hasInputLink = false;
        bool hasOutputLink = false;
        
        for (const auto& link : m_Links) {
            // このノードの入力ピンへのリンクがあるか
            for (const auto& inPin : node.inputs) if (link.endPinId == inPin.id) hasInputLink = true;
            // このノードの出力ピンからのリンクがあるか
            for (const auto& outPin : node.outputs) if (link.startPinId == outPin.id) hasOutputLink = true;
        }

        if (node.className == "Entry") {
            if (!hasOutputLink) {
                node.hasError = true;
                node.validationError += "- Root Entry is not connected to any node.\n";
            }
        }
        else {
            if (!hasInputLink) {
                node.hasError = true;
                node.validationError += "- Node is disconnected (No incoming link).\n";
            }
            
            if ((node.className == "Sequence" || node.className == "Selector" || node.className == "Parallel") && !hasOutputLink) {
                node.hasError = true;
                node.validationError += "- Composite node has no children.\n";
            }
        }

        // --- 2. プロパティチェック ---
        if (node.className == "RunBehaviorNode") {
            if (node.properties["treePath"].empty()) {
                node.hasError = true;
                node.validationError += "- Behavior tree path is not assigned.\n";
            }
        }

        // --- 3. 装飾（Decorator/Service）チェック ---
        for (auto& d : node.decorators) {
            d.hasError = false;
            d.validationError = "";
            if (d.className == "BlackboardDecorator") {
                if (d.properties["keyName"].empty()) {
                    d.hasError = true;
                    d.validationError = "Blackboard key is not assigned.";
                    node.hasError = true;
                    node.validationError += std::format("- Decorator [{}] key is missing.\n", d.name);
                }
            }
        }
        // ... (省略: 他の装飾チェック)
    }

    // --- 4. リンク自体の不整合チェック (Orphaned Links) ---
    for (const auto& link : m_Links) {
        if (allPinIds.find((uintptr_t)link.startPinId) == allPinIds.end() || allPinIds.find((uintptr_t)link.endPinId) == allPinIds.end()) {
            // リンク先またはリンク元のピンが存在しない致命的なエラー
            ONEngine::Console::LogError(std::format("[BT_EDITOR] Orphaned Link Detected! ID:{} Start:{} End:{}", (uintptr_t)link.id, (uintptr_t)link.startPinId, (uintptr_t)link.endPinId));
        }
    }
}

void BehaviorTreeEditorWindow::SaveTree(const std::string& path) {
    json data = GetTreeAsJson();
    std::filesystem::path fsPath(path); if (!std::filesystem::exists(fsPath.parent_path())) std::filesystem::create_directories(fsPath.parent_path());
    std::ofstream file(path); if (file.is_open()) {
        file << data.dump(4);
        m_IsDirty = false;
    }
}

void BehaviorTreeEditorWindow::LoadTree(const std::string& path) {
    std::ifstream file(path); if (!file.is_open()) return;
    json data; try { file >> data; } catch (...) { return; }
    RecordUndo();
    ApplyTreeFromJson(data);
    m_IsDirty = false;
}

}
