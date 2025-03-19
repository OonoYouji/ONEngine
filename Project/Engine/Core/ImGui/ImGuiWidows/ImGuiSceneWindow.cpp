#include "ImGuiSceneWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

void ImGuiSceneWindow::ImGuiFunc() {
	if (!ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove)) {
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
	ImGui::Image(ImTextureID(texture->GetGPUDescriptorHandle().ptr), windowSize);

	ImGui::End();
}
