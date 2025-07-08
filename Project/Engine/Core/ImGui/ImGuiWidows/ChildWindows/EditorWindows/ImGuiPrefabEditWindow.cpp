#include "ImGuiPrefabEditWindow.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"


ImGuiPrefabEditWindow::ImGuiPrefabEditWindow(GraphicsResourceCollection* _resourceCollection)
	: pResourceCollection_(_resourceCollection) {}

void ImGuiPrefabEditWindow::ImGuiFunc() {
	if (!ImGui::Begin("prefab view")) {
		ImGui::End();
		return;
	}


	/// 描画する画像の取得
	const Texture* texture = pResourceCollection_->GetTexture("prefabScene");

	/// 画像の描画

	

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

	// 情報保存
	pImGuiManager_->AddSceneImageInfo("Prefab", ImGuiSceneImageInfo{ imagePos, imageSize });


	ImGui::End();
}
