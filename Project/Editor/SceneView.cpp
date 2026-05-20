#include "SceneView.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Resource/RenderTexture.h"
#include "EditorContext.h"
#include "EditorUtils.h"
#include "Schema/Components.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "ChangeComponentCommand.h"
#include "CommandHistory.h"
#include "Engine/Scene/SceneLoader.h"
#include "Engine/Asset/AssetDatabase.h"
#include <map>
#include <filesystem>
#include <algorithm>

namespace Engine::Editor {

static ImGuizmo::OPERATION s_gizmoOperation = ImGuizmo::TRANSLATE;
static ImGuizmo::MODE s_gizmoMode = ImGuizmo::WORLD;

void SceneView::Render(const char* title, bool* p_open) {
    if (p_open && !*p_open) {
        EditorContext::GetInstance().SetSceneFocused(false);
        return;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(title, p_open);

    auto& context = EditorContext::GetInstance();

    // --- Toolbar ---
    {
        float windowWidth = ImGui::GetWindowWidth();
        float buttonSize = 25.0f;
        // Adjust total width for new buttons
        float totalWidth = buttonSize * 5 + ImGui::GetStyle().ItemSpacing.x * 4;
        ImGui::SetCursorPosX((windowWidth - totalWidth) * 0.5f);

        if (context.IsPlaying()) {
            if (ImGui::Button("||", ImVec2(buttonSize, buttonSize))) { context.SetPaused(!context.IsPaused()); }
        } else {
            if (ImGui::Button(">", ImVec2(buttonSize, buttonSize))) { context.Play(); }
        }
        ImGui::SameLine();
        if (ImGui::Button("X", ImVec2(buttonSize, buttonSize))) { context.Stop(); }

        ImGui::SameLine(0, 20);

        if (ImGui::Button("W", ImVec2(buttonSize, buttonSize))) s_gizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::Button("E", ImVec2(buttonSize, buttonSize))) s_gizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::Button("R", ImVec2(buttonSize, buttonSize))) s_gizmoOperation = ImGuizmo::SCALE;
        ImGui::SameLine();
        if (ImGui::Button((s_gizmoMode == ImGuizmo::LOCAL) ? "Local" : "World", ImVec2(50, buttonSize))) {
            s_gizmoMode = (s_gizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
        }
        ImGui::Separator();
    }
    
    auto& graphics = Engine::Graphics::GraphicsEngine::GetInstance();
    auto* finalBuffer = graphics.GetFinalColorBuffer();
    
    bool isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && !ImGui::IsAnyItemActive();
    context.SetSceneFocused(isFocused);

    if (finalBuffer) {
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = finalBuffer->GetSRVHandle();
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        if (viewportPanelSize.x > 0 && viewportPanelSize.y > 0) {
            ImGui::Image((ImTextureID)srvHandle.ptr, viewportPanelSize);

            if (ImGui::BeginDragDropTarget()) {
                const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATH");
                if (!payload) payload = ImGui::AcceptDragDropPayload("DND_ASSET_PATHS");

                if (payload) {
                    std::string pathStr = (const char*)payload->Data;
                    // DND_ASSET_PATHS の場合は最初のパスのみを抽出 (| で区切られている想定)
                    if (std::string(payload->DataType) == "DND_ASSET_PATHS") {
                        size_t firstSep = pathStr.find('|');
                        if (firstSep != std::string::npos) {
                            pathStr = pathStr.substr(0, firstSep);
                        }
                    }

                    std::filesystem::path path(pathStr);
                    std::string ext = path.extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                    Engine::Console::Log(std::format("SceneView: Dropped asset: {} (ext: {})", pathStr, ext));

                    auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
                    ECS::Entity newEntity = ECS::kNullEntity;

                    if (ext == ".prefab") {
                        newEntity = Engine::Scene::SceneLoader::InstantiatePrefab(pathStr, registry);
                    } else if (ext == ".obj" || ext == ".fbx") {
                        newEntity = registry.CreateEntity();
                        
                        auto& tag = registry.AddComponent<Engine::ECS::Tag>(newEntity);
                        strcpy_s(tag.name, sizeof(tag.name), path.stem().string().c_str());
                        tag.isActive = 1;

                        auto& transform = registry.AddComponent<Engine::ECS::Transform>(newEntity);
                        transform.isEnabled = 1;
                        
                        auto& mesh = registry.AddComponent<Engine::ECS::MeshRenderer>(newEntity);
                        mesh.isEnabled = 1;
                        mesh.modelGuid = Engine::Asset::AssetDatabase::GetInstance().GetGuidFromPath(pathStr);
                    }

                    if (newEntity != ECS::kNullEntity) {
                        context.SetSelectedEntity(newEntity);
                        Engine::Console::Log(std::format("SceneView: Successfully created entity {} from {}", (uint32_t)newEntity, pathStr));
                    } else {
                        Engine::Console::LogError(std::format("SceneView: Failed to create entity from {}", pathStr));
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }

        const auto& selection = context.GetSelection();
        auto primaryEntity = context.GetSelectedEntity();
        auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
        
        if (primaryEntity != 0 && registry.HasComponent<ECS::Transform>(primaryEntity)) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

            auto& camera = context.GetCamera();
            const auto& view = camera.GetViewMatrix();
            const auto& proj = camera.GetProjMatrix();
            
            static bool s_isDragging = false;
            static std::vector<ChangeComponentCommand::EntityState> s_oldStates;
            static std::map<ECS::Entity, Math::Matrix4x4> s_startMatrices;
            
            Math::Matrix4x4 primaryMatrix = registry.GetComponent<ECS::Transform>(primaryEntity).world;

            if (ImGuizmo::IsUsing() && !s_isDragging) {
                s_isDragging = true;
                s_oldStates.clear();
                s_startMatrices.clear();
                for (auto e : selection) {
                    if (registry.HasComponent<ECS::Transform>(e)) {
                        s_oldStates.push_back({ e, ECS::ComponentRegistry::GetInstance().SerializeComponent(registry, e, 1), {} });
                        s_startMatrices[e] = registry.GetComponent<ECS::Transform>(e).world;
                        registry.GetComponent<ECS::Transform>(e).isManipulating = 1;
                    }
                }
            }

            float snapValues[3] = {0,0,0};
            bool useSnap = context.GetSnapEnabled();
            if (s_gizmoOperation == ImGuizmo::TRANSLATE) { snapValues[0] = snapValues[1] = snapValues[2] = context.GetSnapTranslation(); }
            else if (s_gizmoOperation == ImGuizmo::ROTATE) { snapValues[0] = snapValues[1] = snapValues[2] = context.GetSnapRotation(); }
            else if (s_gizmoOperation == ImGuizmo::SCALE) { snapValues[0] = snapValues[1] = snapValues[2] = context.GetSnapScale(); }

            if (ImGuizmo::Manipulate(&view.m[0][0], &proj.m[0][0], s_gizmoOperation, s_gizmoMode, &primaryMatrix.m[0][0], NULL, useSnap ? snapValues : NULL))
            {
                const Math::Matrix4x4& primaryStartMatrix = s_startMatrices[primaryEntity];
                
                // 1st Pass: Update all world matrices based on the delta of the primary entity
                for (auto e : selection) {
                    if (!registry.HasComponent<ECS::Transform>(e)) continue;

                    Math::Matrix4x4 newWorldMatrix;
                    if (s_gizmoMode == ImGuizmo::LOCAL) {
                        // Local Delta: Applied on the left in row-major (v * Delta * World)
                        Math::Matrix4x4 deltaMatrix = primaryMatrix * primaryStartMatrix.Inverse();
                        newWorldMatrix = deltaMatrix * s_startMatrices[e];
                    } else {
                        // World Delta: Applied on the right in row-major (v * World * Delta)
                        Math::Matrix4x4 deltaMatrix = primaryStartMatrix.Inverse() * primaryMatrix;
                        newWorldMatrix = s_startMatrices[e] * deltaMatrix;
                    }

                    auto& targetTransform = registry.GetComponent<ECS::Transform>(e);
                    targetTransform.world = newWorldMatrix;
                }

                // 2nd Pass: Back-calculate local properties from the updated world matrices
                for (auto e : selection) {
                    if (!registry.HasComponent<ECS::Transform>(e)) continue;

                    auto& targetTransform = registry.GetComponent<ECS::Transform>(e);
                    Math::Matrix4x4 newWorldMatrix = targetTransform.world;

                    if (targetTransform.parent != 0 && registry.HasComponent<ECS::Transform>(targetTransform.parent)) {
                         auto& parentTransform = registry.GetComponent<ECS::Transform>(targetTransform.parent);
                         // Local = World * ParentWorld^-1
                         Math::Matrix4x4 localMatrix = newWorldMatrix * parentTransform.world.Inverse();
                         targetTransform.position = localMatrix.ExtractTranslation();
                         targetTransform.rotation = Math::Quaternion::Normalize(localMatrix.ExtractRotation());
                         targetTransform.scale = localMatrix.ExtractScale();
                    } else {
                        targetTransform.position = newWorldMatrix.ExtractTranslation();
                        targetTransform.rotation = Math::Quaternion::Normalize(newWorldMatrix.ExtractRotation());
                        targetTransform.scale = newWorldMatrix.ExtractScale();
                    }
                }
            }
            
            if (!ImGuizmo::IsUsing() && s_isDragging) {
                s_isDragging = false;
                
                std::vector<ChangeComponentCommand::EntityState> finalStates;
                bool changed = false;
                for (const auto& os : s_oldStates) {
                    auto& transform = registry.GetComponent<ECS::Transform>(os.entity);
                    transform.isManipulating = 0;
                    
                    json currentJson = ECS::ComponentRegistry::GetInstance().SerializeComponent(registry, os.entity, 1);
                    if (os.oldState.dump() != currentJson.dump()) {
                        changed = true;
                    }
                    finalStates.push_back({ os.entity, os.oldState, currentJson });
                }
                if (changed) {
                    CommandHistory::GetInstance().Execute(std::make_shared<ChangeComponentCommand>(1, finalStates));
                }
            }
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver()) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 imagePos = ImGui::GetItemRectMin();
            ImVec2 imageSize = ImGui::GetItemRectSize();

            float relX = (mousePos.x - imagePos.x) / imageSize.x;
            float relY = (mousePos.y - imagePos.y) / imageSize.y;

            auto* idBuffer = graphics.GetIDBuffer();
            if (idBuffer) {
                int px = static_cast<int>(relX * idBuffer->GetSize().x);
                int py = static_cast<int>(relY * idBuffer->GetSize().y);

                if (px >= 0 && px < idBuffer->GetSize().x && py >= 0 && py < idBuffer->GetSize().y) {
                    uint32_t entityID = graphics.ReadbackPixel(idBuffer, { px, py });
                    Engine::Console::Log(std::format("Mouse Picking: Pixel({}, {}) -> EntityID={}", px, py, entityID));
                    
                    if (entityID != 0xFFFFFFFF) {
                        EditorContext::GetInstance().SetSelectedEntity(entityID);
                    } else {
                        EditorContext::GetInstance().SetSelectedEntity(0);
                    }
                }
            }
        }
    }

    EditorUtils::DrawActiveViewOutline();
    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace Engine::Editor
