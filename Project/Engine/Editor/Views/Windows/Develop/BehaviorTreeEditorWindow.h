#pragma once

#include "Engine/Editor/Views/EditorViewCollection.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <deque>

// imgui-node-editor
#include <imgui-node-editor/imgui_node_editor.h>

// engine
#include "Engine/Script/MonoScriptEngine.h"

// external
#include <nlohmann/json.hpp>

namespace ONEngine {
    class EntityComponentSystem;
}

namespace Editor {

namespace ed = ax::NodeEditor;

class BehaviorTreeEditorWindow : public IEditorWindow {
public:
    BehaviorTreeEditorWindow(const std::string& title, ONEngine::EntityComponentSystem* ecs);
    ~BehaviorTreeEditorWindow() override;

    void ShowImGui() override;

    // 実行状態の更新（C#から呼ばれる）
    void UpdateNodeStatus(uint32_t nodeIdHash, int status, const std::string& treePath);
    void UpdateBlackboardValue(uint32_t keyHash, const std::string& value, const std::string& typeName);

    static BehaviorTreeEditorWindow* s_Instance;

private:
    enum class PinKind { Input, Output };

    struct Node;

    struct Pin {
        ed::PinId id;
        std::string name;
        PinKind kind;
        Node* node;

        Pin(int _id, const std::string& _name, PinKind _kind)
            : id(_id), name(_name), kind(_kind), node(nullptr) {}
    };

    struct Node {
        ed::NodeId id;
        uint32_t runtimeId = 0; // JSON上のID（C#側との同期用）
        std::string name;
        std::string className;
        std::vector<Pin> inputs;
        std::vector<Pin> outputs;
        ImColor color;
        ImVec2 size;
        std::map<std::string, std::string> properties;
        bool isDecorator = false;
        bool hasBreakpoint = false;

        struct Module {
            uint32_t id;
            std::string className;
            std::string name;
            std::map<std::string, std::string> properties;
            bool isService = false;
            std::string validationError;
            bool hasError = false;
        };
        std::vector<Module> decorators;
        std::vector<Module> services;

        std::string validationError;
        bool hasError = false;

        Node(int _id, const std::string& _name, ImColor _color = ImColor(255, 255, 255))
            : id(_id), name(_name), color(_color) {}
    };

    struct Link {
        ed::LinkId id;
        ed::PinId startPinId;
        ed::PinId endPinId;
        ImColor color;

        Link(ed::LinkId _id, ed::PinId _start, ed::PinId _end)
            : id(_id), startPinId(_start), endPinId(_end), color(255, 255, 255) {}
    };

    struct CommentBox {
        uint32_t id;
        std::string text;
        ImVec2 pos;
        ImVec2 size;
        ImColor color = ImColor(255, 255, 255, 30);
    };

    enum class BBVarType { Int, Float, Bool, Vector3, String };
    struct BBVariable {
        std::string key;
        BBVarType type = BBVarType::Float;
        int iVal = 0;
        float fVal = 0.0f;
        bool bVal = false;
        float vVal[3] = { 0,0,0 };
        char sVal[128] = "";
    };

    struct ModuleClassInfo {
        std::string fullName;
        bool isService = false;
    };

    void InitializeEditor();
    void DrawNodeList();
    void DrawGraphEditor();
    void DrawBlackboardEditor();
    void DrawFileBrowser();
    void DrawNodeInspector();
    void ValidateNodes();
    
    void RefreshFileList();
    void SaveTree(const std::string& path);
    void LoadTree(const std::string& path);

    // Undo/Redo support
    nlohmann::json GetTreeAsJson();
    void ApplyTreeFromJson(const nlohmann::json& data);
    void RecordUndo();
    void Undo();
    void Redo();

    // Productivity features
    void CopySelectedNodes();
    void PasteNodes();
    void DuplicateSelectedNodes();
    
    Node* CreateNode(const std::string& className, bool isDecorator = false);
    void CreateLink(ed::PinId startPin, ed::PinId endPin);

    bool m_EditorInitialized = false;
    ONEngine::EntityComponentSystem* pEcs_;
    std::vector<ONEngine::MonoScriptEngine::NodeClassInfo> availableNodeClasses_;
    std::vector<ModuleClassInfo> availableModuleClasses_;
    std::string m_CurrentFilePath = "";
    
    ed::EditorContext* m_Editor = nullptr;
    std::vector<Node> m_Nodes;
    std::vector<Link> m_Links;
    std::vector<CommentBox> m_CommentBoxes;
    std::vector<BBVariable> m_BBVariables;
    std::map<uint32_t, int> m_RuntimeNodeStatuses;
    std::map<uint32_t, float> m_NodeLastActiveTime;
    
    struct RuntimeBBValue {
        std::string value;
        std::string typeName;
    };
    std::map<uint32_t, RuntimeBBValue> m_RuntimeBBValues;

    std::deque<nlohmann::json> m_UndoStack;
    std::deque<nlohmann::json> m_RedoStack;
    const size_t m_MaxUndoSteps = 50;

    std::string m_Clipboard;

    ed::NodeId m_SelectedNodeId = 0;
    ed::PinId m_NewNodeLinkPinId = 0;
    ImVec2 m_ContextNodePos;
    int m_NextId = 1;

    std::string m_WindowTitle;
    std::vector<std::string> m_AvailableTrees;
    bool m_IsDirty = false;

    float m_LastAutoSaveTime = 0.0f;
    const float m_AutoSaveInterval = 60.0f; // 1 minute for better safety in dev

    int GetNextId() { return m_NextId++; }
};

}
