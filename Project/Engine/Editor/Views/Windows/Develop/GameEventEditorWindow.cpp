#include "GameEventEditorWindow.h"
#include <imgui.h>
#include "Engine/Core/Utility/Utility.h"
#include <cstring>

namespace Editor {

    GameEventEditorWindow::GameEventEditorWindow(const std::string& _name)
        : m_Name(_name) {
        ONEngine::GameEventManager::GetInstance().Initialize();
    }

    void GameEventEditorWindow::ShowImGui() {
        if (!ImGui::Begin(m_Name.c_str())) {
            ImGui::End();
            return;
        }

        if (ImGui::Button("Save to JSON")) {
            ONEngine::GameEventManager::GetInstance().Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload from JSON")) {
            ONEngine::GameEventManager::GetInstance().Load();
        }

        if (ImGui::BeginTabBar("EventTabs")) {
            if (ImGui::BeginTabItem("Attacks")) {
                DrawAttackEditor();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Animations")) {
                DrawAnimationEditor();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    void GameEventEditorWindow::DrawAttackEditor() {
        auto& attacks = ONEngine::GameEventManager::GetInstance().GetAttacks();

        ImGui::BeginChild("AttackList", ImVec2(200, 0), true);
        for (auto& [name, def] : attacks) {
            if (ImGui::Selectable(name.c_str(), m_SelectedAttack == name)) {
                m_SelectedAttack = name;
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("AttackDetails");
        if (!m_SelectedAttack.empty() && attacks.count(m_SelectedAttack)) {
            auto& def = attacks[m_SelectedAttack];
            
            ImGui::Text("Edit Attack: %s", m_SelectedAttack.c_str());
            ImGui::Separator();

            bool changed = false;
            changed |= ImGui::InputFloat("Damage", &def.damage);
            changed |= ImGui::InputFloat("Radius", &def.radius);
            changed |= ImGui::InputFloat("Duration", &def.duration);
            
            float offsetF[3] = { def.offsetForward.x, def.offsetForward.y, def.offsetForward.z };
            if (ImGui::DragFloat3("Offset Forward", offsetF, 0.1f)) {
                def.offsetForward = ONEngine::Vector3(offsetF[0], offsetF[1], offsetF[2]);
                changed = true;
            }

            float offsetU[3] = { def.offsetUp.x, def.offsetUp.y, def.offsetUp.z };
            if (ImGui::DragFloat3("Offset Up", offsetU, 0.1f)) {
                def.offsetUp = ONEngine::Vector3(offsetU[0], offsetU[1], offsetU[2]);
                changed = true;
            }

            if (changed) {
                ONEngine::GameEventManager::GetInstance().SetDirty(true);
            }

            if (ImGui::Button("Delete Attack")) {
                ONEngine::GameEventManager::GetInstance().RemoveAttack(m_SelectedAttack);
                ONEngine::GameEventManager::GetInstance().SetDirty(true);
                m_SelectedAttack = "";
            }
        } else {
            ImGui::Text("Select an attack or create a new one.");
        }

        ImGui::Separator();
        ImGui::InputText("New Attack Name", m_AttackNameBuf, sizeof(m_AttackNameBuf));
        if (ImGui::Button("Create New Attack")) {
            std::string newName = m_AttackNameBuf;
            if (!newName.empty() && !attacks.count(newName)) {
                ONEngine::AttackDefinition def;
                def.name = newName;
                ONEngine::GameEventManager::GetInstance().AddAttack(def);
                ONEngine::GameEventManager::GetInstance().SetDirty(true); // NEW: Set dirty flag
                m_SelectedAttack = newName;
                m_AttackNameBuf[0] = '\0';
            }
        }
        ImGui::EndChild();
    }

    void GameEventEditorWindow::DrawAnimationEditor() {
        auto& animations = ONEngine::GameEventManager::GetInstance().GetAnimations();

        ImGui::BeginChild("AnimList", ImVec2(200, 0), true);
        for (auto& [name, def] : animations) {
            if (ImGui::Selectable(name.c_str(), m_SelectedAnimation == name)) {
                m_SelectedAnimation = name;
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("AnimDetails");
        if (!m_SelectedAnimation.empty() && animations.count(m_SelectedAnimation)) {
            auto& def = animations[m_SelectedAnimation];

            ImGui::Text("Edit Animation Event: %s", m_SelectedAnimation.c_str());
            ImGui::Separator();

            bool changed = false;
            char clipBuf[128];
            strcpy_s(clipBuf, sizeof(clipBuf), def.clipName.c_str());
            if (ImGui::InputText("Clip Name", clipBuf, sizeof(clipBuf))) {
                def.clipName = clipBuf;
                changed = true;
            }

            changed |= ImGui::Checkbox("Loop", &def.loop);
            changed |= ImGui::SliderFloat("Speed", &def.speed, 0.0f, 3.0f);
            changed |= ImGui::InputFloat("CrossFade Time", &def.crossFadeTime);

            if (changed) {
                ONEngine::GameEventManager::GetInstance().SetDirty(true);
            }

            if (ImGui::Button("Delete Event")) {
                ONEngine::GameEventManager::GetInstance().RemoveAnimation(m_SelectedAnimation);
                ONEngine::GameEventManager::GetInstance().SetDirty(true);
                m_SelectedAnimation = "";
            }
        } else {
            ImGui::Text("Select an event or create a new one.");
        }

        ImGui::Separator();
        ImGui::InputText("New Event Name", m_AnimNameBuf, sizeof(m_AnimNameBuf));
        if (ImGui::Button("Create New Event")) {
            std::string newName = m_AnimNameBuf;
            if (!newName.empty() && !animations.count(newName)) {
                ONEngine::AnimationDefinition def;
                def.name = newName;
                ONEngine::GameEventManager::GetInstance().AddAnimation(def);
                ONEngine::GameEventManager::GetInstance().SetDirty(true); // NEW: Set dirty flag
                m_SelectedAnimation = newName;
                m_AnimNameBuf[0] = '\0';
            }
        }
        ImGui::EndChild();
    }

}
