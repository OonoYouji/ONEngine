#include "ImGuiTexturePreviewWindow.h"

using namespace ONEngine;

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

		/// ウィンドウのサイズに合わせてテクスチャのサイズを調整する場合
		ImVec2 availRegion = ImGui::GetContentRegionAvail();

		// アスペクト比に合わせてサイズ調整
		float aspectRatio = 16.0f / 9.0f;
		ImVec2 imageSize = availRegion;
		if (imageSize.x / imageSize.y > aspectRatio) {
			imageSize.x = imageSize.y * aspectRatio;
		} else {
			imageSize.y = imageSize.x / aspectRatio;
		}

		// 位置計算（注意：availRegion を使う）
		ImVec2 windowPos = ImGui::GetCursorScreenPos();
		ImVec2 imagePos = windowPos;
		imagePos.x += (availRegion.x - imageSize.x) * 0.5f;
		imagePos.y += (availRegion.y - imageSize.y) * 0.5f;

		
		/// ----- テクスチャのプレビュー表示 ----- ///
		ImGui::Image(reinterpret_cast<ImTextureID>(texture->GetSRVHandle().
			gpuHandle.ptr), imageSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

	}


	ImGui::End();
}
