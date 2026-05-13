#include "SceneView.h"
#include "imgui.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Resource/RenderTexture.h"
#include "EditorContext.h"

namespace Engine::Editor {

void SceneView::Render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Scene View");

    auto& graphics = Engine::Graphics::GraphicsEngine::GetInstance();
    auto* finalBuffer = graphics.GetFinalColorBuffer();
    if (finalBuffer) {
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = finalBuffer->GetSRVHandle();
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        if (viewportPanelSize.x > 0 && viewportPanelSize.y > 0) {
            ImGui::Image((ImTextureID)srvHandle.ptr, viewportPanelSize);
        }

        // マウスピッキング
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 windowPos = ImGui::GetItemRectMin();
            ImVec2 windowSize = ImGui::GetItemRectSize();

            // ウィンドウ内の相対座標 (0.0 ~ 1.0)
            float relX = (mousePos.x - windowPos.x) / windowSize.x;
            float relY = (mousePos.y - windowPos.y) / windowSize.y;

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

    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace Engine::Editor
