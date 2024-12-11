#include "DemoObject.h"


/// std
#include <format>

#include "SceneManager/SceneManager.h"

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
		&pTransform_->position, &pTransform_->rotate, &pTransform_->scale, 
		ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD
	);

}

void DemoObject::ImGuiGizmo(Vec3* _position, Vec3* _rotate, Vec3* _scale, ImGuizmo::OPERATION _operation, ImGuizmo::MODE _mode) {
	Mat4 matTransform = Mat4::MakeAffine(*_scale, *_rotate, *_position);

	const Vec2& rectMin = SceneManager::GetInstance()->GetSceneRectMin();
	const Vec2& rectMax = SceneManager::GetInstance()->GetSceneRectMax();

	ImGui::Begin("Scene");
	
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);


	CameraManager* cameraManager = CameraManager::GetInstance();
	BaseCamera* camera = cameraManager->GetMainCamera();

	ImGuizmo::Manipulate(
		&camera->GetMatView().m[0][0], // 視点行列（使用しない場合はnullptr）
		&camera->GetMatProjection().m[0][0], // プロジェクション行列（使用しない場合はnullptr）
		_operation, // 操作タイプ（移動、回転、スケーリングなど）
		_mode, // 操作モード（ローカルまたはワールド）
		&matTransform.m[0][0]  // 操作対象の行列
	);

	ImGui::End();

	/// 座標を行列から抽出
	*_position = { matTransform.m[3][0], matTransform.m[3][1], matTransform.m[3][2] };

	/// 回転率を行列から抽出
	*_rotate = Mat4::ExtractEuler(matTransform);
	
	/// 拡縮率を行列から抽出
	_scale->x = std::sqrt(matTransform.m[0][0] + matTransform.m[0][1] + matTransform.m[0][2]);
	_scale->y = std::sqrt(matTransform.m[1][0] + matTransform.m[1][1] + matTransform.m[1][2]);
	_scale->z = std::sqrt(matTransform.m[2][0] + matTransform.m[2][1] + matTransform.m[2][2]);

}

