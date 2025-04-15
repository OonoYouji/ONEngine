#include "ImGuiGameSceneWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"


void ImGuiGameSceneWindow::ImGuiFunc() {
	if (!ImGui::Begin("GameScene")) {
		ImGui::End();
		return;
	}

	const auto& textures = resourceCollection_->GetTextures();
	auto texture = textures[resourceCollection_->GetTextureIndex("scene")].get();

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
	ImGui::Image(ImTextureID(texture->GetSRVGPUHandle().ptr), windowSize);

	ImGui::End();
}
