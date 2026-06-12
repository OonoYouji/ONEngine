#pragma once

#include "Engine/Editor/Views/EditorViewCollection.h"
#include "Engine/Core/Event/GameEventData.h"
#include <string>

namespace Editor {

    class GameEventEditorWindow : public IEditorWindow {
    public:
        GameEventEditorWindow(const std::string& _name);
        ~GameEventEditorWindow() override = default;

        void ShowImGui() override;

    private:
        void DrawAttackEditor();
        void DrawAnimationEditor();

        std::string m_Name;
        std::string m_SelectedAttack;
        std::string m_SelectedAnimation;

        // 編集用バッファ
        char m_AttackNameBuf[128] = "";
        char m_AnimNameBuf[128] = "";
    };

}
