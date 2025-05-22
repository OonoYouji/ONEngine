#define NOMINMAX
#include "TerrainEditor.h"

/// std
#include <fstream>
#include <filesystem>

/// externals
#include <imgui.h>
#include <nlohmann/json.hpp>

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

	editMode_ = 0;
	editFunctions_.resize(EDIT_MODE_SPLAT_BLEND + 1);
	editFunctions_[EDIT_MODE_NONE] = [this]() { /* Do nothing */ };
	editFunctions_[EDIT_MODE_TERRAIN_VERTEX] = [this]() { EditTerrainVertex(); };
	editFunctions_[EDIT_MODE_SPLAT_BLEND] = [this]() { EditSplatBlend(); };
}

void TerrainEditor::Update() {
	mousePosition_ = Input::GetImGuiImageMousePosition("Scene");
	if (Input::TriggerKey(DIK_SPACE)) {
		points_ = pTerrain_->vertices_;
	}
	mouseNearPos_ = CalculateMouseNearPoint(mousePosition_);
	mouseFarPos_ = CalculateMouseFarPoint(mousePosition_);

	if (Input::PressKey(DIK_UPARROW)) {
		editRadius_ += 0.1f;
	}

	if (Input::PressKey(DIK_DOWNARROW)) {
		editRadius_ -= 0.1f;
	}


	if (Input::PressKey(DIK_V)) {
		editMode_ = EDIT_MODE_TERRAIN_VERTEX;
	}
	if (Input::PressKey(DIK_B)) {
		editMode_ = EDIT_MODE_SPLAT_BLEND;
	}

	editFunctions_[editMode_]();


	//Gizmo::DrawRay(mouseNearPos_, mouseFarPos_ - mouseNearPos_);
	Gizmo::DrawWireSphere(mouseNearPos_, 10.0f, Color::kRed);
	//Gizmo::DrawWireSphere(mouseFarPos_, 2.0f, Color::kGreen);



	//if (Input::TriggerKey(DIK_O)) {
	//	OutputVertices("Packages/Jsons/Terrain/TerrainVertices.json");
	//}

}

void TerrainEditor::RecalculateNormal() {

	///// 法線の再計算
	//for (auto& vertex : editedVertices_) {
	//	vertex->normal = Vector3(0.0f, 0.0f, 0.0f);
	//	// 周囲の点を取得
	//	std::vector<Mesh::VertexData*> neighbors;
	//	pTerrain_->GetOctree()->QuerySphere(Vector4::Convert(vertex->position), 1.0f, &neighbors);
	//	// 法線を計算
	//	for (const auto& neighbor : neighbors) {
	//		Vector3&& edge = Vector4::Convert(neighbor->position) - Vector4::Convert(vertex->position);
	//		vertex->normal += edge;
	//	}

	//	vertex->normal = vertex->normal.Normalize();
	//}

}


void TerrainEditor::OutputVertices(const std::string& _filePath) {
	// Ensure the directory exists
	std::filesystem::path dirPath = std::filesystem::path(_filePath).parent_path();
	if (!std::filesystem::exists(dirPath)) {
		std::filesystem::create_directories(dirPath);
	}

	nlohmann::json jsonVertices;

	for (const auto& vertex : pTerrain_->GetVertices()) {
		jsonVertices.push_back({
			{ "position", { vertex.position.x, vertex.position.y, vertex.position.z } }
			//{ "normal", { vertex.normal.x, vertex.normal.y, vertex.normal.z } },
			//{ "uv", { vertex.uv.x, vertex.uv.y } }
			});
	}

	std::ofstream outFile(_filePath);
	if (outFile.is_open()) {
		outFile << jsonVertices.dump(4); // 4 is the indentation level for pretty printing  
		outFile.close();
	} else {
		Assert(false, ("Failed to open file for writing: " + _filePath).c_str());
	}
}

void TerrainEditor::EditTerrainVertex() {

	// mouseNearPos_ と mouseFarPos_ の間にある AABB を構築
	Vec3 min = {
		std::min(mouseNearPos_.x, mouseFarPos_.x),
		std::min(mouseNearPos_.y, mouseFarPos_.y),
		std::min(mouseNearPos_.z, mouseFarPos_.z)
	};
	Vec3 max = {
		std::max(mouseNearPos_.x, mouseFarPos_.x),
		std::max(mouseNearPos_.y, mouseFarPos_.y),
		std::max(mouseNearPos_.z, mouseFarPos_.z)
	};
	Vec3 center = (min + max) * 0.5f;
	Vec3 halfSize = (max - min) * 0.5f;
	AABB rayAABB = { center, halfSize };



	std::vector<std::pair<size_t, TerrainVertex*>> candidates;
	TerrainQuadTree* octree = pTerrain_->GetQuadTree();

	/// 最近接点の候補を計算
	Vec3 rayDir = Vector3::Normalize(mouseFarPos_ - mouseNearPos_);
	TerrainVertex* closestPoint = nullptr;
	float minDist = std::numeric_limits<float>::max();
	float threshold = 5.0f;

	octree->Query(rayAABB, candidates);
	for (const auto& point : candidates) {
		Vec4 v = point.second->position;
		// レイ上の最近接点との距離を計算
		Vec3 toPoint = Vector3(v.x, v.y, v.z) - mouseNearPos_;
		float t = Vector3::Dot(toPoint, rayDir); // レイ上の位置
		if (t < 0.0f) continue; // レイの後ろは無視

		Vec3 closest = mouseNearPos_ + rayDir * t;
		float dist = (closest - Vector3(v.x, v.y, v.z)).Len();

		if (dist < threshold && t < minDist) {
			minDist = t;
			closestPoint = point.second;


			Gizmo::DrawRay(
				mouseNearPos_,
				toPoint.Normalize() * 100.0f,
				Color::kBlue
			);
		}
	}


	/// 最近接点から地形との当たり判定を取る
	editedVertices_.clear();
	if (closestPoint) {

		octree->QuerySphere(
			Vector3(closestPoint->position.x, closestPoint->position.y, closestPoint->position.z),
			editRadius_, &editedVertices_
		);

		Gizmo::DrawWireSphere(
			Vector3(closestPoint->position.x, closestPoint->position.y, closestPoint->position.z) / closestPoint->position.w,
			editRadius_
		);

	}


	/// 衝突している点を上げる
	if (Input::PressMouse(Mouse::Left) || Input::PressKey(DIK_RETURN)) {
		for (auto& point : editedVertices_) {
			Vector4& v = point.second->position;
			v.y += 0.1f;
		}
		RecalculateNormal();
	}

	pTerrain_->editVertices_ = editedVertices_;

}

void TerrainEditor::EditSplatBlend() {

	/// スプラットブレンドモードの変更
	std::array<int, 4> keys = {
		DIK_1, DIK_2, DIK_3, DIK_4
	};

	for (int i = 0; i < keys.size(); i++) {
		if (Input::TriggerKey(keys[i])) {
			editSplatBlendMode_ = i;
		}
	}


	// mouseNearPos_ と mouseFarPos_ の間にある AABB を構築
	Vec3 min = {
		std::min(mouseNearPos_.x, mouseFarPos_.x),
		std::min(mouseNearPos_.y, mouseFarPos_.y),
		std::min(mouseNearPos_.z, mouseFarPos_.z)
	};
	Vec3 max = {
		std::max(mouseNearPos_.x, mouseFarPos_.x),
		std::max(mouseNearPos_.y, mouseFarPos_.y),
		std::max(mouseNearPos_.z, mouseFarPos_.z)
	};
	Vec3 center = (min + max) * 0.5f;
	Vec3 halfSize = (max - min) * 0.5f;
	AABB rayAABB = { center, halfSize };



	std::vector<std::pair<size_t, TerrainVertex*>> candidates;
	TerrainQuadTree* octree = pTerrain_->GetQuadTree();

	/// 最近接点の候補を計算
	Vec3 rayDir = Vector3::Normalize(mouseFarPos_ - mouseNearPos_);
	TerrainVertex* closestPoint = nullptr;
	float minDist = std::numeric_limits<float>::max();
	float threshold = 5.0f;

	octree->Query(rayAABB, candidates);
	for (const auto& point : candidates) {
		Vec4 v = point.second->position;
		// レイ上の最近接点との距離を計算
		Vec3 toPoint = Vector3(v.x, v.y, v.z) - mouseNearPos_;
		float t = Vector3::Dot(toPoint, rayDir); // レイ上の位置
		if (t < 0.0f) continue; // レイの後ろは無視

		Vec3 closest = mouseNearPos_ + rayDir * t;
		float dist = (closest - Vector3(v.x, v.y, v.z)).Len();

		if (dist < threshold && t < minDist) {
			minDist = t;
			closestPoint = point.second;


			Gizmo::DrawRay(
				mouseNearPos_,
				toPoint.Normalize() * 100.0f,
				Color::kBlue
			);
		}
	}


	/// 最近接点から地形との当たり判定を取る
	editedVertices_.clear();
	if (closestPoint) {

		octree->QuerySphere(
			Vector3(closestPoint->position.x, closestPoint->position.y, closestPoint->position.z),
			editRadius_, &editedVertices_
		);

		Gizmo::DrawWireSphere(
			Vector3(closestPoint->position.x, closestPoint->position.y, closestPoint->position.z) / closestPoint->position.w,
			editRadius_
		);

	}


	if (Input::PressMouse(Mouse::Left) || Input::PressMouse(Mouse::Right)) {
		float value = 0.1f;
		if (Input::PressMouse(Mouse::Right)) {
			value = -value;
		}

		for (auto& point : editedVertices_) {
			Vector4& blend = point.second->splatBlend;
			// スプラットブレンドの値を変更
			switch (editSplatBlendMode_) {
			case BLEND_MODE_GRASS:
				blend.x = std::clamp(blend.x + value, 0.0f, 1.0f);
				break;
			case BLEND_MODE_DIRT:
				blend.y = std::clamp(blend.y + value, 0.0f, 1.0f);
				break;
			case BLEND_MODE_ROCK:
				blend.z = std::clamp(blend.z + value, 0.0f, 1.0f);
				break;
			case BLEND_MODE_SNOW:
				blend.w = std::clamp(blend.w + value, 0.0f, 1.0f);
				break;
			}
		}
	}

	pTerrain_->editVertices_ = editedVertices_;

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
