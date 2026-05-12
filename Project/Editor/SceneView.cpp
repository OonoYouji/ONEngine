#include "SceneView.h"
#include "imgui.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Resource/RenderTexture.h"

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
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace Engine::Editor
