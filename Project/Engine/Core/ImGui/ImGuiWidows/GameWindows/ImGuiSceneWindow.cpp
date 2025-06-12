#include "ImGuiSceneWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Core/ImGui/ImGuiManager.h"

void ImGuiSceneWindow::ImGuiFunc() {
	if (!ImGui::Begin("Scene")) {
		ImGui::End();
		return;
	}

	const auto& textures = resourceCollection_->GetTextures();
	auto texture = textures[resourceCollection_->GetTextureIndex("debugScene")].get();

	// 最初に空き領域を取得
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

	// カーソル位置をセットし描画
	ImGui::SetCursorScreenPos(imagePos);
	ImGui::Image(ImTextureID(texture->GetSRVGPUHandle().ptr), imageSize);

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::Text("Window Size: %.1f x %.1f", windowSize.x, windowSize.y);

	// 情報保存
	pImGuiManager_->AddSceneImageInfo("Scene", ImGuiSceneImageInfo{ imagePos, imageSize });

	ImGui::End();

}
