#include "ImGuiSceneWindow.h"

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

/// editor
#include "Engine/Editor/Manager/ImGuiManager.h"
#include "Engine/Editor/Views/ImGuiSelection.h"
#include "ImGuiInspectorWindow.h"

using namespace Editor;

DebugSceneView::DebugSceneView(ONEngine::EntityComponentSystem* _ecs, ONEngine::AssetCollection* _assetCollection, ONEngine::SceneManager* _sceneManager, InspectorWindow* _inspector)
	: pEcs_(_ecs), pAssetCollection_(_assetCollection), pSceneManager_(_sceneManager), pInspector_(_inspector) {

	manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE; // 初期操作モードは移動
	manipulateMode_ = ImGuizmo::MODE::WORLD; // 初期モードはワールド座標
}


void DebugSceneView::ShowImGui() {
	if (!ImGui::Begin("Scene")) {
		ImGui::End();
		return;
	}

	const auto& textures = pAssetCollection_->GetTextures();
	const ONEngine::Texture* texture = &textures[pAssetCollection_->GetTextureIndex("./Assets/Scene/RenderTexture/debugScene")];

	/// ----------------------------------------
	/// ゲームの開始、停止、ポーズボタンの描画
	/// ----------------------------------------

	std::array<const ONEngine::Texture*, 2> buttons = {
		&textures[pAssetCollection_->GetTextureIndex("./Packages/Textures/ImGui/play.png")],
		&textures[pAssetCollection_->GetTextureIndex("./Packages/Textures/ImGui/pause.png")]
	};

	ImVec2 buttonSize = ImVec2(12.0f, 12.0f);
	bool isGameDebug = ONEngine::DebugConfig::isDebugging;

	if (isGameDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.125f, 0.263f, 0.388f, 1.0f));
	}

	ONEngine::MonoScriptEngine::GetInstance().SetIsHotReloadRequest(false);
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
		ONEngine::DebugConfig::isDebugging = false;
	}

	ImGui::SameLine();

	/// DebugConfig::
	if (ImGui::Checkbox("show debug scene", &ONEngine::DebugConfig::isShowDebugScene)) {
		ONEngine::Console::Log("ImGuiSceneWindow::ShowImGui -> clicked show debug scene");
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
	const ONEngine::Guid& selectedGuid = ImGuiSelection::GetSelectedObject();
	if (ONEngine::GameEntity* entity = pEcs_->GetCurrentGroup()->GetEntityFromGuid(selectedGuid)) {

		ImGuizmo::SetOrthographic(false); // 透視投影
		ImGuizmo::SetDrawlist();          // ImGuiの現在のDrawListに出力

		// ウィンドウサイズに合わせて設定
		ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

		/// 操作モードの選択
		if (ONEngine::Input::TriggerKey(DIK_W)) {
			manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE; // 移動
		} else if (ONEngine::Input::TriggerKey(DIK_E)) {
			manipulateOperation_ = ImGuizmo::OPERATION::ROTATE; // 回転
		} else if (ONEngine::Input::TriggerKey(DIK_R)) {
			manipulateOperation_ = ImGuizmo::OPERATION::SCALE; // 拡縮
		} else if (ONEngine::Input::TriggerKey(DIK_Q)) {
			manipulateOperation_ = 0; // 操作なし
		}

		/// モードの選択
		if (ONEngine::Input::TriggerKey(DIK_1)) {
			manipulateMode_ = ImGuizmo::MODE::WORLD; // ワールド座標
		} else if (ONEngine::Input::TriggerKey(DIK_2)) {
			manipulateMode_ = ImGuizmo::MODE::LOCAL; // ローカル座標
		}

		if (manipulateOperation_ != 0) {

			ONEngine::Transform* transform = entity->GetTransform();
			/// 操作対象の行列
			ONEngine::Matrix4x4 entityMatrix = transform->matWorld;

			/// カメラの取得
			ONEngine::CameraComponent* camera = pEcs_->GetECSGroup("Debug")->GetMainCamera();
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

					ONEngine::Vector3 translationV = ONEngine::Vector3(translation[0], translation[1], translation[2]);
					if (ONEngine::GameEntity* owner = transform->GetOwner()) {
						if (ONEngine::GameEntity* parent = owner->GetParent()) {
							translationV = ONEngine::Matrix4x4::Transform(translationV, parent->GetTransform()->GetMatWorld().Inverse());
						}
					}
					transform->SetPosition(translationV);

					ONEngine::Vector3 eulerRotation = ONEngine::Vector3(rotation[0] * ONEngine::Math::Deg2Rad, rotation[1] * ONEngine::Math::Deg2Rad, rotation[2] * ONEngine::Math::Deg2Rad);
					transform->SetRotate(eulerRotation);
					transform->SetScale(ONEngine::Vector3(scale[0], scale[1], scale[2]));

					transform->Update();
				}

			}
		}

	}


	ImGui::End();

}

void DebugSceneView::SetGamePlay(bool _isGamePlay) {
	ONEngine::DebugConfig::isDebugging = _isGamePlay;

	/// 共通の処理（ゲーム開始、停止時に行う処理）
	pSceneManager_->ReloadScene(true);
	ImGuiSelection::SetSelectedObject(ONEngine::Guid::kInvalid, SelectionType::None);

	/// ゲームの開始処理
	if (ONEngine::DebugConfig::isDebugging) {
		pSceneManager_->SaveCurrentSceneTemporary();
		/// Monoスクリプトエンジンのホットリロードでスクリプトの初期化を行う
		ONEngine::MonoScriptEngine::GetInstance().HotReload();
	}
}

