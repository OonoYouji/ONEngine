#include "TerrainEditor.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Entity/Camera/Camera.h"
#include "Engine/ECS/Component/Component.h"


TerrainEditor::TerrainEditor(Terrain* _terrain, EntityComponentSystem* _ecs)
	: pTerrain_(_terrain), pECS_(_ecs) {

	Assert(pTerrain_ != nullptr, "TerrainEditor::Initialize: pTerrain_ is null");
	Assert(pECS_ != nullptr, "TerrainEditor::Initialize: pECS_ is null");
}

TerrainEditor::~TerrainEditor() {}



void TerrainEditor::Initialize() {
	points_ = pTerrain_->vertices_;

}

void TerrainEditor::Update() {
	mousePosition_ = Input::GetImGuiImageMousePosition("Scene");
	if (Input::TriggerKey(DIK_SPACE)) {
		points_ = pTerrain_->vertices_;
	}
	Camera* camera = pECS_->GetDebugCamera();
	Vector3 nearPos = CalculateMouseNearPoint(mousePosition_);
	Vector3 farPos = CalculateMouseFarPoint(mousePosition_);

	std::string text = "CollisionCheck::RayVsSphere == false";
	if (CollisionCheck::RayVsSphere(nearPos, farPos - nearPos,
		Vec3(points_[0].position.x, points_[0].position.y, points_[0].position.z), 1.0f)) {
		text = "CollisionCheck::RayVsSphere == true";

	}

	//for (auto& vertex : points_) {
	//	if (CollisionCheck::RayVsSphere(camera->GetWorldPosition(), rayDirection, Vec3(vertex.position.x, vertex.position.y, vertex.position.z), 1.0f)) {
	//		//vertex.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	//		text = "CollisionCheck::RayVsSphere == true";
	//	}
	//}

	Gizmo::DrawSphere(nearPos, 1.0f, Color::kRed);
	Gizmo::DrawSphere(farPos, 1.0f, Color::kBlue);

	ImGui::Text(text.c_str());
	ImGui::Text("MousePosition: (%f, %f)", mousePosition_.x, mousePosition_.y);
	//ImGui::Text("RayDirection: (%f, %f, %f)", rayDirection.x, rayDirection.y, rayDirection.z);
}

Vector3 TerrainEditor::CalculateMouseRayDirection(const Vector2& _mousePosition) const {
	/// カメラのビュー行列とプロジェクション行列を取得  
	Camera* mainCamera = pECS_->GetDebugCamera();

	const Matrix4x4& viewMatrix = mainCamera->GetViewMatrix();
	const Matrix4x4& projectionMatrix = mainCamera->GetProjectionMatrix();

	Matrix4x4 inversVPVMatrix = Matrix4x4::MakeInverse(viewMatrix * projectionMatrix);
	// スクリーンサイズ（あとで動的にしてもOK）
	float screenWidth = 1280.0f;
	float screenHeight = 720.0f;

	// スクリーン座標 → NDC（-1〜1）
	float ndcX = (_mousePosition.x / screenWidth) * 2.0f - 1.0f;
	float ndcY = 1.0f - (_mousePosition.y / screenHeight) * 2.0f;

	// NDC空間の near / far 点を作成（z=0: near, z=1: far）
	Vector3 ndcNear(ndcX, ndcY, 0.0f);
	Vector3 ndcFar(ndcX, ndcY, 1.0f);

	// ワールド空間へ逆変換（w除算込み）
	ndcNear = Matrix4x4::Transform(ndcNear, inversVPVMatrix);
	ndcFar = Matrix4x4::Transform(ndcFar, inversVPVMatrix);

	// レイ方向（正規化）
	Vector3 rayDir = Vector3::Normalize(ndcFar - ndcNear);

	return rayDir;
}

Vector3 TerrainEditor::CalculateMouseNearPoint(const Vector2& _mousePosition) const {
	/// カメラのビュー行列とプロジェクション行列を取得  
	Camera* mainCamera = pECS_->GetDebugCamera();

	const Matrix4x4& viewMatrix = mainCamera->GetViewMatrix();
	const Matrix4x4& projectionMatrix = mainCamera->GetProjectionMatrix();

	Matrix4x4 inversVPVMatrix = Matrix4x4::MakeInverse(viewMatrix * projectionMatrix);
	// スクリーンサイズ（あとで動的にしてもOK）
	float screenWidth = 1280.0f;
	float screenHeight = 720.0f;

	// スクリーン座標 → NDC（-1〜1）
	float ndcX = (_mousePosition.x / screenWidth) * 2.0f - 1.0f;
	float ndcY = 1.0f - (_mousePosition.y / screenHeight) * 2.0f;

	// NDC空間の near / far 点を作成（z=0: near, z=1: far）
	Vector3 ndcNear(ndcX, ndcY, 0.0f);

	// ワールド空間へ逆変換（w除算込み）
	return Matrix4x4::Transform(ndcNear, inversVPVMatrix);
}

Vector3 TerrainEditor::CalculateMouseFarPoint(const Vector2& _mousePosition) const {
	/// カメラのビュー行列とプロジェクション行列を取得  
	Camera* mainCamera = pECS_->GetDebugCamera();

	const Matrix4x4& viewMatrix = mainCamera->GetViewMatrix();
	const Matrix4x4& projectionMatrix = mainCamera->GetProjectionMatrix();

	Matrix4x4 inversVPVMatrix = Matrix4x4::MakeInverse(viewMatrix * projectionMatrix);
	// スクリーンサイズ（あとで動的にしてもOK）
	float screenWidth = 1280.0f;
	float screenHeight = 720.0f;

	// スクリーン座標 → NDC（-1〜1）
	float ndcX = (_mousePosition.x / screenWidth) * 2.0f - 1.0f;
	float ndcY = 1.0f - (_mousePosition.y / screenHeight) * 2.0f;

	// NDC空間の near / far 点を作成（z=0: near, z=1: far）
	Vector3 ndcFar(ndcX, ndcY, 1.0f);

	// ワールド空間へ逆変換（w除算込み）
	return Matrix4x4::Transform(ndcFar, inversVPVMatrix);
}
