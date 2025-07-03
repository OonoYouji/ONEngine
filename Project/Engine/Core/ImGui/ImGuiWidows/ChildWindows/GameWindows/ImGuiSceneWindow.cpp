#include "ImGuiSceneWindow.h"

/// std
#include <array>

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
	Texture* texture = textures[resourceCollection_->GetTextureIndex("debugScene")].get();

	/// ----------------------------------------
	/// ゲームの開始、停止、ポーズボタンの描画
	/// ----------------------------------------

	std::array<Texture*, 3> buttons = {
		textures[resourceCollection_->GetTextureIndex("./Packages/Textures/ImGui/play.png")].get(),
		textures[resourceCollection_->GetTextureIndex("./Packages/Textures/ImGui/pause.png")].get(),
		textures[resourceCollection_->GetTextureIndex("./Packages/Textures/ImGui/skip.png")].get()
	};

	ImVec2 buttonSize = ImVec2(12.0f, 12.0f);
	bool isGameDebug = pImGuiManager_->GetIsGameDebug();
	if (isGameDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.125f, 0.263f, 0.388f, 1.0f));
	}

	if (ImGui::ImageButton("##play", ImTextureID(buttons[0]->GetSRVGPUHandle().ptr), buttonSize)) {
		// デバッグモードを開始
		pImGuiManager_->SetIsGameDebug(!isGameDebug);
	}
	ImGui::SameLine();

	if (isGameDebug) {
		ImGui::PopStyleColor(1);
	}

	/// 一時停止ボタン
	if (ImGui::ImageButton("##pause", ImTextureID(buttons[1]->GetSRVGPUHandle().ptr), buttonSize)) {
		// デバッグモードを停止
		pImGuiManager_->SetIsGameDebug(false);
	}
	ImGui::SameLine();

	/// 1frameスキップボタン
	if (ImGui::ImageButton("##skip", ImTextureID(buttons[2]->GetSRVGPUHandle().ptr), buttonSize)) {

	}



	ImGui::Separator();


	/// ----------------------------------------
	/// ゲームシーンの描画
	/// ----------------------------------------

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
	pImGuiManager_->AddSceneImageInfo("Scene", ImGuiSceneImageInfo{ imagePos, imageSize });

	ImGui::End();

}
