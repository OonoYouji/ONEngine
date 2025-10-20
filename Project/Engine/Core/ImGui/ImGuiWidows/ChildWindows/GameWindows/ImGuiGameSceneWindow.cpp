#include "ImGuiGameSceneWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Core/ImGui/ImGuiManager.h"


void ImGuiGameSceneWindow::ShowImGui() {
	if (!ImGui::Begin("GameScene")) {
		ImGui::End();
		return;
	}

	/// ---------------------------------------
	/// Sceneを描画する
	/// ---------------------------------------

	const auto& textures = pGrc_->GetTextures();
	auto& texture = textures[pGrc_->GetTextureIndex("sceneScene")];

	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	float aspectRatio = 16.0f / 9.0f;
	if (windowSize.x / windowSize.y > aspectRatio) {
		windowSize.x = windowSize.y * aspectRatio;
	} else {
		windowSize.y = windowSize.x / aspectRatio;
	}

	ImVec2 windowPos = ImGui::GetCursorScreenPos();
	ImVec2 imagePos = windowPos;
	imagePos.x += (ImGui::GetContentRegionAvail().x - windowSize.x) * 0.5f;
	imagePos.y += (ImGui::GetContentRegionAvail().y - windowSize.y) * 0.5f;

	ImGui::SetCursorScreenPos(imagePos);
	ImGui::Image(ImTextureID(texture.GetSRVGPUHandle().ptr), windowSize);

	pImGuiManager_->AddSceneImageInfo("GameScene", ImGuiSceneImageInfo{ imagePos, windowSize });

	ImGui::End();
}
