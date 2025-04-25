#include "TerrainEditor.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Entity/Camera/Camera.h"


TerrainEditor::TerrainEditor(Terrain* _terrain, EntityComponentSystem* _ecs)
	: pTerrain_(_terrain), pECS_(_ecs) {

	Assert(pTerrain_ != nullptr, "TerrainEditor::Initialize: pTerrain_ is null");
	Assert(pECS_ != nullptr, "TerrainEditor::Initialize: pECS_ is null");
}

TerrainEditor::~TerrainEditor() {}



void TerrainEditor::Initialize() {

}

void TerrainEditor::Update() {
	mousePosition_ = Input::GetImGuiImageMousePosition("Scene");
	if (Input::TriggerMouse(Mouse::Left)) {
		points_ = pTerrain_->vertices_;
	}

	Vector3 rayDirection = CalculateMouseRayDirection(Vec2(640.0f, 360.0f));

	//ImGui::Text(std::format(
		//"TerrainEditor::Update: mousePosition: {}, \n rayDirection: {}, {}, {}", mousePosition_, rayDirection.x, rayDirection.y, rayDirection.z).c_str());
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
