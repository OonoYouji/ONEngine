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

	/// チャンクとの当たり判定をとる
	std::list<TerrainChunk*> collidedChunks;
	for (auto& chunk : pTerrain_->chunks_) {
		const Vector3& chunkPosition = chunk.GetPosition();
		const Vector2& chunkSize     = chunk.GetChunkSize();

		if (CollisionCheck::RayVsCube(nearPos, Vector3::Normalize(farPos - nearPos),
			chunkPosition, Vector3(chunkSize.x, 1.0f, chunkSize.y))) {
			collidedChunks.push_back(&chunk);
		}
	}

	std::list<Vector4*> hitPoints;

	/// 衝突しているチャンクを使って当たり判定を計算する
	for (auto& chunk : collidedChunks) {

		const std::vector<Mesh::VertexData*>& vertices = chunk->GetChunkVertices();
		for (auto& vertex : vertices) {

			if (CollisionCheck::RayVsSphere(
				nearPos, Vector3::Normalize(farPos - nearPos),
				Vec3(vertex->position.x, vertex->position.y, vertex->position.z), 3.0f)) {
				hitPoints.push_back(&vertex->position);
			}
		}
	}


	if (Input::PressMouse(Mouse::Left) || Input::PressKey(DIK_RETURN)) {
		for (auto& point : hitPoints) {
			point->y += 0.1f;
		}
	}

	ImGui::Text("MousePosition: (%f, %f)", mousePosition_.x, mousePosition_.y);
	ImGui::Text("MouseScreenPosition: (%f, %f)", Input::GetMousePosition().x, Input::GetMousePosition().y);
	ImGui::Text("Mouse Left Button: %s", Input::PressMouse(Mouse::Left) ? "Pressed" : "Released");
	ImGui::Text("Mouse Right Button: %s", Input::PressMouse(Mouse::Right) ? "Pressed" : "Released");


	for (auto& hitPoint : hitPoints) {
		ImGui::Text("HitPoint: (%f, %f, %f)", hitPoint->x, hitPoint->y, hitPoint->z);
	}



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
