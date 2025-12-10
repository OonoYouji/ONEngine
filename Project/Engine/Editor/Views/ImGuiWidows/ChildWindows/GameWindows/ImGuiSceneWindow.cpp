﻿#include "ImGuiSceneWindow.h"

using namespace ONEngine;

/// std
#include <array>

/// external
#include <imgui.h>
#include <ImGuizmo.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/MonoScriptEngine.h"

/// engine/imgui
#include "Engine/Core/ImGui/ImGuiSelection.h"
#include "ImGuiInspectorWindow.h"

ImGuiSceneWindow::ImGuiSceneWindow(EntityComponentSystem* _ecs, AssetCollection* _assetCollection, SceneManager* _sceneManager, ImGuiInspectorWindow* _inspector)
	: pEcs_(_ecs), pAssetCollection_(_assetCollection), pSceneManager_(_sceneManager), pInspector_(_inspector) {

	manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE; // 初期操作モードは移動
	manipulateMode_ = ImGuizmo::MODE::WORLD; // 初期モードはワールド座標
}


void ImGuiSceneWindow::ShowImGui() {
	if (!ImGui::Begin("Scene")) {
		ImGui::End();
		return;
	}

	const auto& textures = pAssetCollection_->GetTextures();
	const Texture* texture = &textures[pAssetCollection_->GetTextureIndex("./Assets/Scene/RenderTexture/debugScene")];

	/// ----------------------------------------
	/// ゲームの開始、停止、ポーズボタンの描画
	/// ----------------------------------------

	std::array<const Texture*, 2> buttons = {
		&textures[pAssetCollection_->GetTextureIndex("./Packages/Textures/ImGui/play.png")],
		&textures[pAssetCollection_->GetTextureIndex("./Packages/Textures/ImGui/pause.png")]
	};

	ImVec2 buttonSize = ImVec2(12.0f, 12.0f);
	bool isGameDebug = DebugConfig::isDebugging;

	if (isGameDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.125f, 0.263f, 0.388f, 1.0f));
	}

	MonoScriptEngine::GetInstance().SetIsHotReloadRequest(false);
	if (ImGui::ImageButton("##play", ImTextureID(buttons[0]->GetSRVGPUHandle().ptr), buttonSize)) {
		SetGamePlay(!isGameDebug); // ゲームプレイの開始/停止
	}
	ImGui::SameLine();

	if (isGameDebug) {
		ImGui::PopStyleColor(1);
	}

	/// 一時停止ボタン
	if (ImGui::ImageButton("##pause", ImTextureID(buttons[1]->GetSRVGPUHandle().ptr), buttonSize)) {
		// デバッグモードを停止
		DebugConfig::isDebugging = false;
	}

	ImGui::SameLine();

	/// DebugConfig::
	if (ImGui::Checkbox("show debug scene", &DebugConfig::isShowDebugScene)) {
		Console::Log("ImGuiSceneWindow::ShowImGui -> clicked show debug scene");
	}

	/// ----------------------------------------
	/// ImGuiInfoをTextに表示
	/// ----------------------------------------
	{
		ImGui::SameLine();

		const std::string& text = ImGuiInfo::GetInfo();
		float textWidth = ImGui::CalcTextSize(text.c_str()).x;
		float windowWidth = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(windowWidth - textWidth);

		ImGui::TextColored(ImVec4(0.75f, 0, 0, 1), text.c_str());
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




	/// ----------------------------------------
	/// gizmoの表示
	/// ----------------------------------------

	/// Guidを元に操作対象のエンティティを取得する、
	const Guid& selectedGuid = ImGuiSelection::GetSelectedObject();
	if (GameEntity* entity = pEcs_->GetCurrentGroup()->GetEntityFromGuid(selectedGuid)) {

		ImGuizmo::SetOrthographic(false); // 透視投影
		ImGuizmo::SetDrawlist();          // ImGuiの現在のDrawListに出力

		// ウィンドウサイズに合わせて設定
		ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

		/// 操作モードの選択
		if (Input::TriggerKey(DIK_W)) {
			manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE; // 移動
		} else if (Input::TriggerKey(DIK_E)) {
			manipulateOperation_ = ImGuizmo::OPERATION::ROTATE; // 回転
		} else if (Input::TriggerKey(DIK_R)) {
			manipulateOperation_ = ImGuizmo::OPERATION::SCALE; // 拡縮
		} else if (Input::TriggerKey(DIK_Q)) {
			manipulateOperation_ = 0; // 操作なし
		}

		/// モードの選択
		if (Input::TriggerKey(DIK_1)) {
			manipulateMode_ = ImGuizmo::MODE::WORLD; // ワールド座標
		} else if (Input::TriggerKey(DIK_2)) {
			manipulateMode_ = ImGuizmo::MODE::LOCAL; // ローカル座標
		}

		if (manipulateOperation_ != 0) {

			Transform* transform = entity->GetTransform();
			/// 操作対象の行列
			Matrix4x4 entityMatrix = transform->matWorld;

			/// カメラの取得
			CameraComponent* camera = pEcs_->GetECSGroup("Debug")->GetMainCamera();
			if (camera) {
				ImGuizmo::Manipulate(
					&camera->GetViewMatrix().m[0][0],
					&camera->GetProjectionMatrix().m[0][0],
					ImGuizmo::OPERATION(manipulateOperation_), // TRANSLATE, ROTATE, SCALE
					ImGuizmo::MODE(manipulateMode_), // WORLD or LOCAL
					&entityMatrix.m[0][0]
				);

				if (ImGuizmo::IsUsing() && ImGuizmo::IsOver()) {
					/// 行列をSRTに分解、エンティティに適応
					float translation[3], rotation[3], scale[3];
					ImGuizmo::DecomposeMatrixToComponents(&entityMatrix.m[0][0], translation, rotation, scale);

					Vector3 translationV = Vector3(translation[0], translation[1], translation[2]);
					if (GameEntity* owner = transform->GetOwner()) {
						if (GameEntity* parent = owner->GetParent()) {
							translationV = Matrix4x4::Transform(translationV, parent->GetTransform()->GetMatWorld().Inverse());
						}
					}
					transform->SetPosition(translationV);

					Vector3 eulerRotation = Vector3(rotation[0] * Mathf::Deg2Rad, rotation[1] * Mathf::Deg2Rad, rotation[2] * Mathf::Deg2Rad);
					transform->SetRotate(eulerRotation);
					transform->SetScale(Vector3(scale[0], scale[1], scale[2]));

					transform->Update();
				}

			}
		}

	}


	ImGui::End();

}

void ImGuiSceneWindow::SetGamePlay(bool _isGamePlay) {
	DebugConfig::isDebugging = _isGamePlay;

	if (DebugConfig::isDebugging) {
		//!< ゲームの開始処理
		pSceneManager_->SaveCurrentSceneTemporary();

		pSceneManager_->ReloadScene(true);
		ImGuiSelection::SetSelectedObject(Guid::kInvalid, SelectionType::None);

		/// Monoスクリプトエンジンのホットリロードでスクリプトの初期化を行う
		MonoScriptEngine::GetInstance().HotReload();

	} else {
		//!< 更新処理を停止した場合の処理
		pSceneManager_->ReloadScene(true);
		ImGuiSelection::SetSelectedObject(Guid::kInvalid, SelectionType::None);

	}

}

