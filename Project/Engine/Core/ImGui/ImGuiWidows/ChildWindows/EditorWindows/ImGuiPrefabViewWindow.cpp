#include "ImGuiPrefabViewWindow.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

ImGuiPrefabViewWindow::ImGuiPrefabViewWindow(EntityComponentSystem* _ecs, AssetCollection* _assetCollection)
	: pEcs_(_ecs), pAssetCollection_(_assetCollection) {}

void ImGuiPrefabViewWindow::ShowImGui() {
	if (!ImGui::Begin("prefab view")) {
		ImGui::End();
		return;
	}


	/// カメラの更新
	CameraComponent* debugCamera = pEcs_->GetECSGroup("Debug")->GetMainCamera();
	if (debugCamera) {
		//debugCamera->Update();
	}

	//GameEntity* prefabEntity = pECS_->GetECSGroup()->GetPrefabEntity();
	//if (prefabEntity) {
	//	prefabEntity->UpdateTransform();
	//	//pECS_->RuntimeUpdateSystems();
	//	prefabEntity->UpdateTransform();
	//}


	/// オブジェクトの正面にカメラを配置
	if (Input::TriggerKey(DIK_F)) {

		Vector3 dir = Vector3(0.0f, 2.0f, -6.5f).Normalize();
		float length = 6.0f; /// オブジェクトとカメラの距離

		if (debugCamera) {
			GameEntity* cameraEntity = debugCamera->GetOwner();
			if (cameraEntity) {
				cameraEntity->SetPosition(dir * length);
				cameraEntity->SetRotate(Vector3(0.05f, 0.0f, 0.0f)); /// オブジェクトの正面を向く
				debugCamera->UpdateViewProjection();
			}
		}

	}


	/// prefabの描画
	RenderView();


	ImGui::End();
}

void ImGuiPrefabViewWindow::RenderView() {

	/// 描画する画像の取得
	const Texture* texture = pAssetCollection_->GetTexture("prefabScene");

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

}
