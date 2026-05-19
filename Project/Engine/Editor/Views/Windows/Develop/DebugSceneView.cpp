#include "DebugSceneView.h"

/// std
#include <array>

/// external
#include <imgui.h>
#include <ImGuizmo.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/Core/DirectX12/GPUTimeStamp/GPUTimeStamp.h"
#include "Engine/Core/Utility/Time/CPUTimeStamp.h"

/// editor
#include "Engine/Editor/EditorUtils.h"
#include "Engine/Editor/Manager/ImGuiManager.h"
#include "Engine/Editor/Math/ImGuiSelection.h"
#include "InspectorWindow.h"

namespace {
template<typename... Args>
std::string Format(const char* fmt, Args... args) {
	int size = std::snprintf(nullptr, 0, fmt, args...) + 1;
	std::string buf(size, '\0');
	std::snprintf(buf.data(), size, fmt, args...);
	buf.pop_back(); // null文字削除
	return buf;
}
}


namespace Editor {

DebugSceneView::DebugSceneView(ONEngine::EntityComponentSystem* _ecs, ONEngine::Asset::AssetCollection* _assetCollection, ONEngine::SceneManager* _sceneManager, InspectorWindow* _inspector)
	: pEcs_(_ecs), pAssetCollection_(_assetCollection), pSceneManager_(_sceneManager), pInspector_(_inspector) {

	manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE; // 初期操作モードは移動
	manipulateMode_ = ImGuizmo::MODE::WORLD; // 初期モードはワールド座標
}


void DebugSceneView::ShowImGui() {
	if(!ImGui::Begin("Scene")) {
		ImGui::End();
		return;
	}

	HandleCameraFocus();
	DrawToolbar();

	ImGui::Separator();

	ImVec2 imagePos, imageSize;
	DrawSceneTexture(imagePos, imageSize);

	DrawGizmoAndOverlays(imagePos, imageSize);

	ImGui::End();
}

void DebugSceneView::SetGamePlay(bool _isGamePlay) {
	ONEngine::DebugConfig::isDebugging = _isGamePlay;

	/// ゲームの開始処理
	if(ONEngine::DebugConfig::isDebugging) {
		pSceneManager_->SaveCurrentSceneTemporary();

		pSceneManager_->ReloadScene(true);
		ImGuiSelection::SetSelectedObject(ONEngine::Guid::kInvalid, SelectionType::None);

		/// Monoスクリプトエンジンのホットリロードでスクリプトの初期化を行う
		ONEngine::MonoScriptEngine::GetInstance().HotReload();
	} else {

		/// 共通の処理（ゲーム開始、停止時に行う処理）
		pSceneManager_->ReloadScene(true);
		ImGuiSelection::SetSelectedObject(ONEngine::Guid::kInvalid, SelectionType::None);
	}

}

void Editor::DebugSceneView::ShowDebugSceneView(const ImVec2& imagePos) {
	std::vector<OverlaySection> sections;

	{
		// 地形描画 セクション
		double regularCellTime = ONEngine::GPUTimeStamp::GetInstance().GetTimeStampMSec(ONEngine::GPUTimeStampID::VoxelTerrainRegularCell); // ms
		double transitionCellTime = ONEngine::GPUTimeStamp::GetInstance().GetTimeStampMSec(ONEngine::GPUTimeStampID::VoxelTerrainTransitionCell); // ms
		double editorComputeTime = ONEngine::GPUTimeStamp::GetInstance().GetTimeStampMSec(ONEngine::GPUTimeStampID::VoxelTerrainEditorCompute); // ms
		double editorComputeBrushPreview = ONEngine::GPUTimeStamp::GetInstance().GetTimeStampMSec(ONEngine::GPUTimeStampID::VoxelTerrainEditorBrushPreview); // ms
		OverlaySection renderer;
		renderer.name = "地形描画";
		renderer.opened = true;
		renderer.items = {
			{ "RegularCell", Format("%f ms", regularCellTime), IM_COL32(255, 255, 255, 255) },
			{ "TransitionCell", Format("%f ms", transitionCellTime), IM_COL32(255, 255, 255, 255) },
			{ "EditorCompute", Format("%f ms", editorComputeTime), IM_COL32(255, 255, 255, 255) },
			{ "BrushPreview", Format("%f ms", editorComputeBrushPreview), IM_COL32(255, 255, 255, 255) },
		};
		sections.push_back(renderer);
	}

	{
		/// C#スクリプト セクション
		double scriptUpdateTime = ONEngine::CPUTimeStamp::GetInstance().GetElapsedTimeMicroseconds(ONEngine::CPUTimeStampID::CSharpScriptUpdate); // マイクロ秒
		OverlaySection renderer;
		renderer.name = "C#スクリプト";
		renderer.opened = true;
		renderer.items = {
			{ "C# Script Update", Format("%f ms", scriptUpdateTime), IM_COL32(255, 255, 255, 255) }
		};
		sections.push_back(renderer);
	}

	// 描画
	DrawSceneOverlayStats(imagePos, sections);
}

void DebugSceneView::DrawSceneOverlayStats(const ImVec2& imagePos, const std::vector<OverlaySection>& sections) {
	ImDrawList* drawList = ImGui::GetForegroundDrawList();

	float y = imagePos.y + 8.0f; // 上マージン
	float x = imagePos.x + 8.0f; // 左マージン

	auto DrawSeparator = [&](const std::vector<OverlayItem>& items)
	{
		float maxWidth = 0.0f;

		// セクション内のテキスト幅を計算して最大値を取得
		for(const auto& item : items) {
			if(!item.visible) continue;
			ImVec2 size = ImGui::CalcTextSize((item.label + " : " + item.value).c_str());
			if(size.x > maxWidth) maxWidth = size.x;
		}

		// 少し余白をつける
		maxWidth += 8.0f;

		drawList->AddLine(
			{ x, y },
			{ x + maxWidth, y },
			IM_COL32(255, 255, 255, 80)
		);

		y += 6.0f; // 線の下にマージン
	};


	auto DrawHeader = [&](const std::string& title, bool opened)
	{
		ImU32 color = opened
			? IM_COL32(255, 220, 120, 255)
			: IM_COL32(180, 180, 180, 255);

		drawList->AddText(
			{ x, y },
			color,
			title.c_str()
		);

		y += ImGui::GetTextLineHeight();
	};

	auto DrawItem = [&](const OverlayItem& item)
	{
		if(!item.visible) return;

		std::string line = item.label + " : " + item.value;
		drawList->AddText(
			{ x, y },
			item.color,
			line.c_str()
		);
		y += ImGui::GetTextLineHeight();
	};

	// 各セクション描画
	for(const auto& section : sections) {
		DrawHeader(section.name, section.opened);
		DrawSeparator(section.items);

		if(section.opened) {
			for(const auto& item : section.items) {
				DrawItem(item);
			}
		}

		y += 6.0f; // セクション間マージン
	}
}

///
/// Fキーによるカメラフォーカス
///
void DebugSceneView::HandleCameraFocus() {
	if(ImGui::IsWindowHovered() || ImGui::IsWindowFocused()) {
		if(ONEngine::Input::TriggerKey(DIK_F)) {
			ONEngine::Guid selectedGuid = ImGuiSelection::GetSelectedObject();
			if(selectedGuid.CheckValid()) {
				ONEngine::GameEntity* targetEntity = pEcs_->GetCurrentGroup()->GetEntityFromGuid(selectedGuid);
				if(targetEntity) {
					ONEngine::Vector3 targetPos = targetEntity->GetPosition();
					ONEngine::CameraComponent* editorCamera = pEcs_->GetECSGroup("Debug")->GetMainCamera();

					if(editorCamera) {
						float distance = 10.0f;
						ONEngine::Vector3 cameraForward = { 0.0f, -0.5f, 1.0f };
						ONEngine::Vector3 newCameraPos;
						newCameraPos.x = targetPos.x - (cameraForward.x * distance);
						newCameraPos.y = targetPos.y - (cameraForward.y * distance);
						newCameraPos.z = targetPos.z - (cameraForward.z * distance);

						ONEngine::Transform* transform = editorCamera->GetOwner()->GetComponent<ONEngine::Transform>();
						transform->SetPosition(newCameraPos);
						editorCamera->LookAt(targetPos - newCameraPos);
						editorCamera->UpdateViewProjection();
					}
				}
			}
		}
	}
}

///
/// ツールバーの表示(再生ボタン、設定チェックボックスなど)
///
void DebugSceneView::DrawToolbar() {
	std::array<const ONEngine::Asset::Texture*, 2> buttons = {
		pAssetCollection_->GetTexture("./Packages/Textures/ImGui/play.png"),
		pAssetCollection_->GetTexture("./Packages/Textures/ImGui/pause.png")
	};

	// dds用フォールバック
	std::array<std::string, 2> paths = {
		"./Packages/Textures/ImGui/play.dds",
		"./Packages/Textures/ImGui/pause.dds"
	};
	for(uint8_t i = 0; i < 2; ++i) {
		if(!buttons[i]) {
			buttons[i] = pAssetCollection_->GetTexture(paths[i]);
		}
	}

	ImVec2 buttonSize = ImVec2(12.0f, 12.0f);
	bool isGameDebug = ONEngine::DebugConfig::isDebugging;

	if(isGameDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.125f, 0.263f, 0.388f, 1.0f));
	}

	ONEngine::MonoScriptEngine::GetInstance().SetIsHotReloadRequest(false);
	if(ImGui::ImageButton("##play", ImTextureID(buttons[0]->GetSRVGPUHandle().ptr), buttonSize)) {
		SetGamePlay(!isGameDebug);
	}
	ImGui::SameLine();

	if(isGameDebug) {
		ImGui::PopStyleColor(1);
	}

	// 一時停止ボタン
	if(ImGui::ImageButton("##pause", ImTextureID(buttons[1]->GetSRVGPUHandle().ptr), buttonSize)) {
		ONEngine::DebugConfig::isDebugging = false;
	}

	ImGui::SameLine();

	// デバッグシーンの表示トグル
	if(ImGui::Checkbox("show debug scene", &ONEngine::DebugConfig::isShowDebugScene)) {
		ONEngine::Console::Log("ImGuiSceneWindow::ShowImGui -> clicked show debug scene");
	}

	ImGui::SameLine();

	// スタッツの表示トグル (メンバ変数に変更)
	ImGui::Checkbox("show scene stats", &isDrawSceneStats_);

	// ImGuiInfo の右寄せ表示
	{
		ImGui::SameLine();
		const std::string& text = ImGuiInfo::GetInfo();
		float textWidth = ImGui::CalcTextSize(text.c_str()).x;
		float windowWidth = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(windowWidth - textWidth);
		ImGui::TextColored(ImVec4(0.75f, 0, 0, 1), text.c_str());
	}
}

///
/// シーンテクスチャの描画と座標計算
///
void DebugSceneView::DrawSceneTexture(ImVec2& outImagePos, ImVec2& outImageSize) {
	const auto& textures = pAssetCollection_->GetTextures();
	const ONEngine::Asset::Texture* texture = &textures[pAssetCollection_->GetTextureIndex("./Assets/Scene/RenderTexture/debugScene")];

	ImVec2 availRegion = ImGui::GetContentRegionAvail();
	float aspectRatio = 16.0f / 9.0f;

	outImageSize = availRegion;
	if(outImageSize.x / outImageSize.y > aspectRatio) {
		outImageSize.x = outImageSize.y * aspectRatio;
	} else {
		outImageSize.y = outImageSize.x / aspectRatio;
	}

	ImVec2 windowPos = ImGui::GetCursorScreenPos();
	outImagePos = windowPos;
	outImagePos.x += (availRegion.x - outImageSize.x) * 0.5f;
	outImagePos.y += (availRegion.y - outImageSize.y) * 0.5f;

	ImGui::SetCursorScreenPos(outImagePos);
	ImGui::Image(ImTextureID(texture->GetSRVGPUHandle().ptr), outImageSize);

	// 情報保存（ギズモのピッキング等に使用）
	pImGuiManager_->AddSceneImageInfo("Scene", ImGuiSceneImageInfo{ outImagePos, outImageSize });
}

///
/// ギズモ操作と統計情報の表示
///
void DebugSceneView::DrawGizmoAndOverlays(const ImVec2& imagePos, const ImVec2& imageSize) {
	Editor::SetEntity(ImGuiSelection::GetSelectedObject());

	ONEngine::Vector2 imagePosV = { imagePos.x, imagePos.y };
	ONEngine::Vector2 imageSizeV = { imageSize.x, imageSize.y };
	Editor::SetDrawRect(imagePosV, imageSizeV);
	Editor::UpdatePivot(pEcs_);

	if(isDrawSceneStats_) {
		ShowDebugSceneView(imagePos);
	}
}

} /// namespace Editor
