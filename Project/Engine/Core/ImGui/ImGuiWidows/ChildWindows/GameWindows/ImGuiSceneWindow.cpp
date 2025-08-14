#include "ImGuiSceneWindow.h"

/// std
#include <array>

/// external
#include <imgui.h>
#include <ImGuizmo.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Component.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Script/MonoScriptEngine.h"

#include "ImGuiInspectorWindow.h"

ImGuiSceneWindow::ImGuiSceneWindow(EntityComponentSystem* _ecs, GraphicsResourceCollection* _graphicsResourceCollection, SceneManager* _sceneManager, ImGuiInspectorWindow* _inspector)
	: pECS_(_ecs), resourceCollection_(_graphicsResourceCollection), pSceneManager_(_sceneManager), pInspector_(_inspector) {

	manipulateOperation_ = ImGuizmo::OPERATION::TRANSLATE; // 初期操作モードは移動
	manipulateMode_ = ImGuizmo::MODE::WORLD; // 初期モードはワールド座標
}


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
	bool isGameDebug = DebugConfig::isDebugging;

	if (isGameDebug) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.125f, 0.263f, 0.388f, 1.0f));
	}

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

	// 操作対象のゲット
	GameEntity* entity = pInspector_->GetSelectedEntity();
	if (entity) {

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
		}

		/// モードの選択
		if (Input::TriggerKey(DIK_1)) {
			manipulateMode_ = ImGuizmo::MODE::WORLD; // ワールド座標
		} else if (Input::TriggerKey(DIK_2)) {
			manipulateMode_ = ImGuizmo::MODE::LOCAL; // ローカル座標
		}


		Transform* transform = entity->GetTransform();
		/// 操作対象の行列
		Matrix4x4 entityMatrix = transform->matWorld;

		/// カメラの取得
		CameraComponent* camera = pECS_->GetECSGroup("Debug")->GetMainCamera();
		if (camera) {
			ImGuizmo::Manipulate(
				&camera->GetViewMatrix().m[0][0],
				&camera->GetProjectionMatrix().m[0][0],
				ImGuizmo::OPERATION(manipulateOperation_), // TRANSLATE, ROTATE, SCALE
				ImGuizmo::MODE(manipulateMode_), // WORLD or LOCAL
				&entityMatrix.m[0][0]
			);

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


	ImGui::End();

}

void ImGuiSceneWindow::SetGamePlay(bool _isGamePlay) {
	DebugConfig::isDebugging = _isGamePlay;

	if (DebugConfig::isDebugging) {
		//!< ゲームの開始処理
		pSceneManager_->SaveCurrentSceneTemporary();

		pSceneManager_->ReloadScene(true);
		pInspector_->SetSelectedEntity(0);


		ComponentArray<Script>* gameScripts = pECS_->GetCurrentGroup()->GetComponentArray<Script>();
		ComponentArray<Script>* debugScripts = pECS_->GetECSGroup("Debug")->GetComponentArray<Script>();

		/// Monoスクリプトエンジンのホットリロードでスクリプトの初期化を行う
		GetMonoScriptEnginePtr()->HotReload();

		/// スクリプトの初期化
		if (gameScripts && gameScripts->GetUsedComponents().size() > 0) {
			for (Script* script : gameScripts->GetUsedComponents()) {
				if (script) {
					script->ResetScripts();
				}
			}
		}

		if (debugScripts && debugScripts->GetUsedComponents().size() > 0) {
			for (Script* script : debugScripts->GetUsedComponents()) {
				if (script) {
					script->ResetScripts();
				}
			}
		}

	} else {
		//!< 更新処理を停止した場合の処理
		pSceneManager_->ReloadScene(true);
		pInspector_->SetSelectedEntity(0);

	}

}

