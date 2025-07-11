#include "ImGuiPrefabViewWindow.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Entities/Camera/DebugCamera.h"

ImGuiPrefabViewWindow::ImGuiPrefabViewWindow(EntityComponentSystem* _ecs, GraphicsResourceCollection* _resourceCollection)
	: pECS_(_ecs), pResourceCollection_(_resourceCollection) {}

void ImGuiPrefabViewWindow::ImGuiFunc() {
	if (!ImGui::Begin("prefab view")) {
		ImGui::End();
		return;
	}


	/// カメラの更新
	Camera* debugCamera = pECS_->GetDebugCamera();
	if (debugCamera) {
		debugCamera->Update();
	}

	IEntity* prefabEntity = pECS_->GetPrefabEntity();
	if (prefabEntity) {
		prefabEntity->Update();
		prefabEntity->UpdateTransform();

		pECS_->UpdateSystems({ prefabEntity });

		prefabEntity->UpdateTransform();
	}


	/// オブジェクトの正面にカメラを配置
	if (Input::TriggerKey(DIK_F)) {

		Vector3 dir = Vector3(0.0f, 2.0f, -6.5f).Normalize();
		float length = 6.0f; /// オブジェクトとカメラの距離

		if (debugCamera) {
			debugCamera->SetPosition(dir * length);
			debugCamera->SetRotate(Vector3(0.05f, 0.0f, 0.0f)); /// オブジェクトの正面を向く
			debugCamera->UpdateViewProjection();
		}

	}


	/// prefabの描画
	RenderView();


	ImGui::End();
}

void ImGuiPrefabViewWindow::RenderView() {

	/// 描画する画像の取得
	const Texture* texture = pResourceCollection_->GetTexture("prefabScene");

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
