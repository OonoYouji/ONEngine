#include "ImGuiGizmo.h"

/// externals
#include <imgui.h>
#include <ImGuizmo.h>

/// engine
#include "Input/Input.h"
#include "SceneManager/SceneManager.h"
#include "Objects/Camera/Manager/CameraManager.h"

namespace {

	/// operation, modeを切り替える
	static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
	static float pivotSize = 0.2f;

}


void ImGuiGizmo(Vec3* _position, Vec3* _rotate, Vec3* _scale, ROTATE_ORDER _rotateOrder) {

	if(Input::TriggerKey(KeyCode::W)) { operation = ImGuizmo::OPERATION::TRANSLATE; }
	if(Input::TriggerKey(KeyCode::E)) { operation = ImGuizmo::OPERATION::ROTATE; }
	if(Input::TriggerKey(KeyCode::R)) { operation = ImGuizmo::OPERATION::SCALE; }

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
	BaseCamera*    camera        = cameraManager->GetMainCamera();

	ImGuizmo::SetGizmoSizeClipSpace(pivotSize);
	ImGuizmo::Manipulate(
		&camera->GetMatView().m[0][0],       /// 視点行列
		&camera->GetMatProjection().m[0][0], /// プロジェクション行列
		operation,                           /// 操作タイプ（移動、回転、スケーリングなど）
		ImGuizmo::MODE::WORLD,               /// 操作モード（ローカルまたはワールド）
		&matTransform.m[0][0]                /// 操作対象の行列
	);

	ImGui::End();

	switch(operation) {
	case ImGuizmo::TRANSLATE:
		/// 座標を行列から抽出
		*_position = { matTransform.m[3][0], matTransform.m[3][1], matTransform.m[3][2] };
		break;
	case ImGuizmo::ROTATE:
		/// 回転率を行列から抽出
		*_rotate = Mat4::ExtractEulerAngles(matTransform, _rotateOrder);
		break;
	case ImGuizmo::SCALE:
		/// 拡縮率を行列から抽出
		_scale->x = Vec3::Lenght(Vec3(matTransform.m[0][0], matTransform.m[0][1], matTransform.m[0][2]));
		_scale->y = Vec3::Lenght(Vec3(matTransform.m[1][0], matTransform.m[1][1], matTransform.m[1][2]));
		_scale->z = Vec3::Lenght(Vec3(matTransform.m[2][0], matTransform.m[2][1], matTransform.m[2][2]));
		break;
	}
}

void ImGuiGizmo(Mat4* _matTransform, Vec3* _position, Vec3* _rotate, Vec3* _scale, ROTATE_ORDER _rotateOrder) {


	if(Input::TriggerKey(KeyCode::W)) { operation = ImGuizmo::OPERATION::TRANSLATE; }
	if(Input::TriggerKey(KeyCode::E)) { operation = ImGuizmo::OPERATION::ROTATE; }
	if(Input::TriggerKey(KeyCode::R)) { operation = ImGuizmo::OPERATION::SCALE; }

	if(Input::TriggerKey(KeyCode::Alpha3)) { pivotSize -= 0.1f; }
	if(Input::TriggerKey(KeyCode::Alpha4)) { pivotSize += 0.1f; }


	/// 操作する行列
	Mat4 matTransform = *_matTransform;

	const Vec2& rectMin = SceneManager::GetInstance()->GetSceneRectMin();
	const Vec2& rectMax = SceneManager::GetInstance()->GetSceneRectMax();

	ImGui::Begin("Debug");

	ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
	ImGuizmo::SetRect(rectMin.x, rectMin.y, rectMax.x, rectMax.y);

	CameraManager* cameraManager = CameraManager::GetInstance();
	BaseCamera* camera = cameraManager->GetMainCamera();

	ImGuizmo::SetGizmoSizeClipSpace(pivotSize);
	ImGuizmo::Manipulate(
		&camera->GetMatView().m[0][0],       /// 視点行列
		&camera->GetMatProjection().m[0][0], /// プロジェクション行列
		operation,                           /// 操作タイプ（移動、回転、スケーリングなど）
		ImGuizmo::MODE::WORLD,               /// 操作モード（ローカルまたはワールド）
		&matTransform.m[0][0]                /// 操作対象の行列
	);

	ImGui::End();

	switch(operation) {
	case ImGuizmo::TRANSLATE:
		/// 座標を行列から抽出
		*_position = { matTransform.m[3][0], matTransform.m[3][1], matTransform.m[3][2] };
		break;
	case ImGuizmo::ROTATE:
		/// 回転率を行列から抽出
		*_rotate = Mat4::ExtractEulerAngles(matTransform, _rotateOrder);
		break;
	case ImGuizmo::SCALE:
		/// 拡縮率を行列から抽出
		_scale->x = Vec3::Lenght(Vec3(matTransform.m[0][0], matTransform.m[0][1], matTransform.m[0][2]));
		_scale->y = Vec3::Lenght(Vec3(matTransform.m[1][0], matTransform.m[1][1], matTransform.m[1][2]));
		_scale->z = Vec3::Lenght(Vec3(matTransform.m[2][0], matTransform.m[2][1], matTransform.m[2][2]));
		break;
	}
}
