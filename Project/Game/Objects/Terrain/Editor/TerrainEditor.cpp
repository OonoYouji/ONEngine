#define NOMINMAX
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
	Vector3 nearPos = CalculateMouseNearPoint(mousePosition_);
	Vector3 farPos = CalculateMouseFarPoint(mousePosition_);

	// nearPos と farPos の間にある AABB を構築
	Vec3 min = {
		std::min(nearPos.x, farPos.x),
		std::min(nearPos.y, farPos.y),
		std::min(nearPos.z, farPos.z)
	};
	Vec3 max = {
		std::max(nearPos.x, farPos.x),
		std::max(nearPos.y, farPos.y),
		std::max(nearPos.z, farPos.z)
	};
	Vec3 center = (min + max) * 0.5f;
	Vec3 halfSize = (max - min) * 0.5f;
	AABB rayAABB = { center, halfSize };

	//Gizmo::DrawRay(nearPos, farPos - nearPos);
	Gizmo::DrawWireSphere(nearPos, 10.0f, Color::kRed);
	//Gizmo::DrawWireSphere(farPos, 2.0f, Color::kGreen);


	if (Input::PressKey(DIK_UPARROW)) {
		editRadius_ += 0.1f;
	}

	if (Input::PressKey(DIK_DOWNARROW)) {
		editRadius_ -= 0.1f;
	}


	std::vector<Mesh::VertexData*> candidates;
	TerrainOctree* octree = pTerrain_->GetOctree();

	/// 最近接点の候補を計算
	Vec3 rayDir = Vector3::Normalize(farPos - nearPos);
	Mesh::VertexData* closestPoint = nullptr;
	float minDist = std::numeric_limits<float>::max();
	float threshold = 5.0f;

	octree->Query(rayAABB, candidates);
	for (const auto& point : candidates) {
		// レイ上の最近接点との距離を計算
		Vec3 toPoint = Vector3(point->position.x, point->position.y, point->position.z) - nearPos;
		float t = Vector3::Dot(toPoint, rayDir); // レイ上の位置
		if (t < 0.0f) continue; // レイの後ろは無視

		Vec3 closest = nearPos + rayDir * t;
		float dist = (closest - Vector3(point->position.x, point->position.y, point->position.z)).Len();

		if (dist < threshold && t < minDist) {
			minDist = t;
			closestPoint = point;


			Gizmo::DrawRay(
				nearPos,
				toPoint.Normalize() * 100.0f,
				Color::kBlue
			);
		}
	}


	/// 最近接点から地形との当たり判定を取る
	std::vector<Mesh::VertexData*> hitPoints;
	if (closestPoint) {

		octree->QuerySphere(
			Vector3(closestPoint->position.x, closestPoint->position.y, closestPoint->position.z), 
			editRadius_, &hitPoints
		);

		Gizmo::DrawWireSphere(
			Vector3(closestPoint->position.x, closestPoint->position.y, closestPoint->position.z) / closestPoint->position.w,
			editRadius_
		);

	}


	/// 衝突している点を上げる
	if (Input::PressMouse(Mouse::Left) || Input::PressKey(DIK_RETURN)) {
		for (auto& point : hitPoints) {
			point->position.y += 0.1f;
		}
	}



	//ImGui::Text("MousePosition: (%f, %f)", mousePosition_.x, mousePosition_.y);
	//ImGui::Text("MouseScreenPosition: (%f, %f)", Input::GetMousePosition().x, Input::GetMousePosition().y);
	//ImGui::Text("Mouse Left Button: %s", Input::PressMouse(Mouse::Left) ? "Pressed" : "Released");
	//ImGui::Text("Mouse Right Button: %s", Input::PressMouse(Mouse::Right) ? "Pressed" : "Released");

	ImGui::Text(std::format("near position: ({}, {}, {})", nearPos.x, nearPos.y, nearPos.z).c_str());
	ImGui::Text(std::format("far position: ({}, {}, {})", farPos.x, farPos.y, farPos.z).c_str());

	//for (auto& hitPoint : hitPoints) {
	//	//ImGui::Text("HitPoint: (%f, %f, %f)", hitPoint->x, hitPoint->y, hitPoint->z);
	//}



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
