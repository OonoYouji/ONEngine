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
	mousePosition_ = Input::GetImGuiImageMousePosition();
	if (Input::TriggerMouse(Mouse::Left)) {
		points_ = pTerrain_->vertices_;
	}

	ImGui::Text(std::format("TerrainEditor::Update: mousePosition: {}, points_: {}", mousePosition_, points_.size()).c_str());
}

Vector3 TerrainEditor::CalculateMouseWorldPosition(const Vector2& _mousePosition) const {
	/// カメラのビュー行列とプロジェクション行列を取得  
	Camera* mainCamera = pECS_->GetMainCamera();

	const Matrix4x4& viewMatrix = mainCamera->GetViewMatrix();
	const Matrix4x4& projectionMatrix = mainCamera->GetProjectionMatrix();

	/// ビューポート座標系でのマウス位置を正規化デバイス座標系に変換  
	//float normalizedX = (_mousePosition.x / Input::GetScreenWidth()) * 2.0f - 1.0f;
	//float normalizedY = 1.0f - (_mousePosition.y / Input::GetScreenHeight()) * 2.0f;
	float normalizedX = (_mousePosition.x / 1280.0f) * 2.0f - 1.0f;
	float normalizedY = 1.0f - (_mousePosition.y / 720.0f) * 2.0f;
	Vector3 mouseNDC(normalizedX, normalizedY, 1.0f);

	/// 逆行列を使用してワールド座標系に変換  
	Matrix4x4 inverseViewProjection = Matrix4x4::MakeInverse(viewMatrix * projectionMatrix);
	Vector3 mouseWorldPosition = Matrix4x4::Transform(mouseNDC, inverseViewProjection);

	/// レイの方向を計算  
	Vector3 cameraPosition = mainCamera->GetWorldPosition();
	Vector3 rayDirection = Vector3::Normalize(mouseWorldPosition - cameraPosition);

	return rayDirection;
}
