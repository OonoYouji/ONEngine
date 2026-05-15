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

namespace Engine::Editor {

static ImGuizmo::OPERATION s_gizmoOperation = ImGuizmo::TRANSLATE;
static ImGuizmo::MODE s_gizmoMode = ImGuizmo::LOCAL;

void SceneView::Render(bool* p_open) {
    if (p_open && !*p_open) {
        EditorContext::GetInstance().SetSceneFocused(false);
        return;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Scene View", p_open);

    auto& context = EditorContext::GetInstance();
    auto& graphics = Engine::Graphics::GraphicsEngine::GetInstance();
    auto* finalBuffer = graphics.GetFinalColorBuffer();
    
    // フォーカス状態を保存 (次フレームのカメラ操作用)
    bool isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && !ImGui::IsAnyItemActive();
    context.SetSceneFocused(isFocused);

    // Gizmo 操作の切り替え
    if (!ImGui::IsAnyItemActive()) {
        if (ImGui::IsKeyPressed(ImGuiKey_W)) s_gizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E)) s_gizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) s_gizmoOperation = ImGuizmo::SCALE;
        if (ImGui::IsKeyPressed(ImGuiKey_Q)) s_gizmoMode = (s_gizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
    }

    if (finalBuffer) {
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = finalBuffer->GetSRVHandle();
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        if (viewportPanelSize.x > 0 && viewportPanelSize.y > 0) {
            ImGui::Image((ImTextureID)srvHandle.ptr, viewportPanelSize);
        }

        // Gizmo
        auto selectedEntity = context.GetSelectedEntity();
        auto& registry = Engine::Core::Application::GetInstance().GetRegistry();
        if (selectedEntity != 0 && registry.HasComponent<ECS::Transform>(selectedEntity)) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

            auto& camera = context.GetCamera();
            const auto& view = camera.GetViewMatrix();
            const auto& proj = camera.GetProjMatrix();
            auto& transform = registry.GetComponent<ECS::Transform>(selectedEntity);

            // ワールド行列を操作
            Math::Matrix4x4 matrix = transform.world;
            
            // Undo/Redo 用の状態管理
            static bool s_isDragging = false;
            static nlohmann::json s_oldState;
            static ECS::Entity s_draggedEntity = 0;

            if (ImGuizmo::IsUsing() && !s_isDragging) {
                // ドラッグ開始: スナップショット作成 (Transform の ID は 1)
                s_isDragging = true;
                s_draggedEntity = selectedEntity;
                s_oldState = ECS::ComponentRegistry::GetInstance().SerializeComponent(registry, selectedEntity, 1);
            }

            // Gizmoの描画と更新
            float snapValues[3];
            if (s_gizmoOperation == ImGuizmo::TRANSLATE) {
                snapValues[0] = snapValues[1] = snapValues[2] = context.GetSnapTranslation();
            } else if (s_gizmoOperation == ImGuizmo::ROTATE) {
                snapValues[0] = snapValues[1] = snapValues[2] = context.GetSnapRotation();
            } else if (s_gizmoOperation == ImGuizmo::SCALE) {
                snapValues[0] = snapValues[1] = snapValues[2] = context.GetSnapScale();
            }

            if (ImGuizmo::Manipulate(&view.m[0][0], &proj.m[0][0], s_gizmoOperation, s_gizmoMode, &matrix.m[0][0], NULL, context.GetSnapEnabled() ? snapValues : NULL)) {
                // 操作中のリアルタイム更新
                Math::Vector3 pos, rot, sca;
                ImGuizmo::DecomposeMatrixToComponents(&matrix.m[0][0], &pos.x, &rot.x, &sca.x);

                if (transform.parent == 0) {
                    transform.position = pos;
                    transform.rotation = rot;
                    transform.scale = sca;
                } else {
                    // 親がいる場合の逆行列計算が必要
                    if (registry.HasComponent<ECS::Transform>(transform.parent)) {
                        auto& parentTransform = registry.GetComponent<ECS::Transform>(transform.parent);
                        Math::Matrix4x4 invParent = parentTransform.world.Inverse();
                        Math::Matrix4x4 localMatrix = matrix * invParent;
                        
                        Math::Vector3 lPos, lRot, lSca;
                        ImGuizmo::DecomposeMatrixToComponents(&localMatrix.m[0][0], &lPos.x, &lRot.x, &lSca.x);
                        transform.position = lPos;
                        transform.rotation = lRot;
                        transform.scale = lSca;
                    }
                }
            }

            if (!ImGuizmo::IsUsing() && s_isDragging) {
                // ドラッグ終了 -> コマンド発行
                s_isDragging = false;
                auto newState = ECS::ComponentRegistry::GetInstance().SerializeComponent(registry, s_draggedEntity, 1);
                
                // 値が変わっている場合のみ履歴に追加
                if (s_oldState != newState) {
                    auto command = std::make_shared<ChangeComponentCommand>(s_draggedEntity, 1, s_oldState, newState);
                    CommandHistory::GetInstance().Execute(command);
                }
            }
        }

        // マウスピッキング
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver()) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 imagePos = ImGui::GetItemRectMin();
            ImVec2 imageSize = ImGui::GetItemRectSize();

            // ウィンドウ内の相対座標 (0.0 ~ 1.0)
            float relX = (mousePos.x - imagePos.x) / imageSize.x;
            float relY = (mousePos.y - imagePos.y) / imageSize.y;

            // テクスチャ座標に変換
            auto* idBuffer = graphics.GetIDBuffer();
            if (idBuffer) {
                int px = static_cast<int>(relX * idBuffer->GetSize().x);
                int py = static_cast<int>(relY * idBuffer->GetSize().y);

                // 範囲外チェック
                if (px >= 0 && px < idBuffer->GetSize().x && py >= 0 && py < idBuffer->GetSize().y) {
                    uint32_t entityID = graphics.ReadbackPixel(idBuffer, { px, py });
                    Engine::Console::Log(std::format("Mouse Picking: Pixel({}, {}) -> EntityID={}", px, py, entityID));
                    
                    if (entityID != 0xFFFFFFFF) {
                        EditorContext::GetInstance().SetSelectedEntity(entityID);
                    } else {
                        // 背景をクリックした場合は選択解除 (0 は無効なEntityとする)
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
