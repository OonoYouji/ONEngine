#include "ImGuiTexturePreviewWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"

ImGuiTexturePreviewWindow::ImGuiTexturePreviewWindow(AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {
	searchFilter_ = "./Assets/Scene/RenderTexture/shadowMapScene";
}

ImGuiTexturePreviewWindow::~ImGuiTexturePreviewWindow() {
}

void ImGuiTexturePreviewWindow::ShowImGui() {
	if(!ImGui::Begin("Texture Preview")) {
		ImGui::End();
		return;
	}


	ImMathf::InputText("Search Filter", &searchFilter_);

	Texture* texture = pAssetCollection_->GetTexture(searchFilter_);
	if (texture) {

		/// ----- テクスチャのプレビュー表示 ----- ///
		ImTextureID imguiTextureID = reinterpret_cast<ImTextureID>(texture->GetSRVHandle().gpuHandle.ptr);
		float aspectRatio = texture->GetTextureSize().x / texture->GetTextureSize().y;
		ImGui::Image(imguiTextureID, ImVec2(256.0f * aspectRatio, 256.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

	}


	ImGui::End();
}
