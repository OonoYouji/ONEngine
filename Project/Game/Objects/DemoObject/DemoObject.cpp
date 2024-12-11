#include "DemoObject.h"


/// std
#include <format>

#include "SceneManager/SceneManager.h"
#include "Input/Input.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Objects/Camera/Manager/CameraManager.h"


void DemoObject::Initialize() {
	renderer_ = AddComponent<MeshRenderer>();
	renderer_->SetModel("Sphere");
	renderer_->SetMaterial("uvChecker.png");
}

void DemoObject::Update() {


}

void DemoObject::Debug() {

	ImGuiGizmo(
		&pTransform_->position, &pTransform_->rotate, &pTransform_->scale
	);

}

void DemoObject::ImGuiGizmo(Vec3* _position, Vec3* _rotate, Vec3* _scale) {

	/// operation, modeを切り替える
	static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
	static ImGuizmo::MODE      mode      = ImGuizmo::MODE::WORLD;
	static float pivotSize = 0.2f;

	if(Input::TriggerKey(KeyCode::W)) { operation = ImGuizmo::OPERATION::TRANSLATE; }
	if(Input::TriggerKey(KeyCode::E)) { operation = ImGuizmo::OPERATION::ROTATE; }
	if(Input::TriggerKey(KeyCode::R)) { operation = ImGuizmo::OPERATION::SCALE; }

	if(Input::TriggerKey(KeyCode::Alpha1)) { mode = ImGuizmo::MODE::WORLD; }
	if(Input::TriggerKey(KeyCode::Alpha2)) { mode = ImGuizmo::MODE::LOCAL; }

	if(Input::TriggerKey(KeyCode::Alpha3)) { pivotSize -= 0.1f; }
	if(Input::TriggerKey(KeyCode::Alpha4)) { pivotSize += 0.1f; }


	/// 操作する行列
	Mat4 matTransform = Mat4::MakeAffine(*_scale, *_rotate, *_position);

	const Vec2& rectMin = SceneManager::GetInstance()->GetSceneRectMin();
	const Vec2& rectMax = SceneManager::GetInstance()->GetSceneRectMax();

	ImGui::Begin("Debug");
	
	ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
	ImGuizmo::SetRect(rectMin.x, rectMin.y, rectMax.x, rectMax.y);

	CameraManager* cameraManager = CameraManager::GetInstance();
	BaseCamera* camera = cameraManager->GetMainCamera();
	
	ImGuizmo::SetGizmoSizeClipSpace(pivotSize);
	ImGuizmo::Manipulate(
		&camera->GetMatView().m[0][0], // 視点行列（使用しない場合はnullptr）
		&camera->GetMatProjection().m[0][0], // プロジェクション行列（使用しない場合はnullptr）
		operation, // 操作タイプ（移動、回転、スケーリングなど）
		mode,      // 操作モード（ローカルまたはワールド）
		&matTransform.m[0][0]  // 操作対象の行列
	);

	ImGui::End();

	// Gizmo の使用状態を確認
	if(ImGuizmo::IsUsing()) {
		ImGui::Text("Gizmo is active!");
	} else {
		ImGui::Text("Gizmo is NOT active!");
	}

	ImGui::Text("rectMin: (%f, %f)", rectMin.x, rectMin.y);
	ImGui::Text("rectMax: (%f, %f)", rectMax.x, rectMax.y);


	/// 座標を行列から抽出
	Mat4 matScaleInverse = {};
	switch(operation) {
	case ImGuizmo::TRANSLATE:
		*_position = { matTransform.m[3][0], matTransform.m[3][1], matTransform.m[3][2] };
		break;
	case ImGuizmo::ROTATE:
		/// 回転率を行列から抽出
		matScaleInverse = Mat4::MakeScale(*_scale).Inverse();
		*_rotate = Mat4::ExtractEuler(matTransform);
		break;
	case ImGuizmo::SCALE:
		/// 拡縮率を行列から抽出
		_scale->x = Vec3::Lenght(Vec3(matTransform.m[0][0], matTransform.m[0][1], matTransform.m[0][2]));
		_scale->y = Vec3::Lenght(Vec3(matTransform.m[1][0], matTransform.m[1][1], matTransform.m[1][2]));
		_scale->z = Vec3::Lenght(Vec3(matTransform.m[2][0], matTransform.m[2][1], matTransform.m[2][2]));
		break;
	}


	
	//_scale->x = std::sqrt(matTransform.m[0][0] + matTransform.m[0][1] + matTransform.m[0][2]);
	//_scale->y = std::sqrt(matTransform.m[1][0] + matTransform.m[1][1] + matTransform.m[1][2]);
	//_scale->z = std::sqrt(matTransform.m[2][0] + matTransform.m[2][1] + matTransform.m[2][2]);
	

}

