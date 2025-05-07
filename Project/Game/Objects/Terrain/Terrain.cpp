#include "Terrain.h"

/// std
#include <fstream>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/Component/Component.h"

Terrain::Terrain() {}
Terrain::~Terrain() {}

void Terrain::Initialize() {

	/// 頂点の生成
	const size_t terrainWidth = static_cast<size_t>(terrainSize_.x);
	const size_t terrainHeight = static_cast<size_t>(terrainSize_.y);
	vertices_.resize(terrainWidth * terrainHeight);

	for (size_t row = 0; row < terrainWidth; ++row) {
		for (size_t col = 0; col < terrainHeight; ++col) {

			size_t index = row * terrainWidth + col;
			vertices_[index].position = Vector4(static_cast<float>(row), 0.0f, static_cast<float>(col), 1.0f);
			vertices_[index].uv = Vector2(
				static_cast<float>(row) / static_cast<float>(terrainWidth),
				static_cast<float>(col) / static_cast<float>(terrainHeight) * -1.0f
			);
			vertices_[index].normal = Vector3(0.0f, 1.0f, 0.0f);
		}
	}


	/// インデックスの生成
	const size_t faceVerts = 6;
	indices_.resize((terrainWidth - 1) * (terrainHeight - 1) * faceVerts);

	for (size_t row = 0; row < terrainWidth - 1; ++row) {
		for (size_t col = 0; col < terrainHeight - 1; ++col) {

			size_t index = row * (terrainHeight - 1) * faceVerts + col * faceVerts;
			indices_[index + 0] = static_cast<uint32_t>(row * terrainWidth + col);
			indices_[index + 1] = static_cast<uint32_t>(row * terrainWidth + col + 1);
			indices_[index + 2] = static_cast<uint32_t>((row + 1) * terrainWidth + col);
			indices_[index + 3] = static_cast<uint32_t>((row + 1) * terrainWidth + col);
			indices_[index + 4] = static_cast<uint32_t>(row * terrainWidth + col + 1);
			indices_[index + 5] = static_cast<uint32_t>((row + 1) * terrainWidth + col + 1);
		}
	}


	/// spanに変換
	vertexSpan_ = std::span<std::span<Mesh::VertexData>>(reinterpret_cast<std::span<Mesh::VertexData>*>(vertices_.data()), terrainWidth);


	/// カスタムメッシュで地形の描画を行う
	CustomMeshRenderer* meshRenderer = AddComponent<CustomMeshRenderer>();
	meshRenderer->SetVertices(vertices_);
	meshRenderer->SetIndices(indices_);
	meshRenderer->SetIsBufferRecreate(true);

	/// Octreeの生成
	Vector3 center = Vector3(terrainSize_.x * 0.5f, 0.0f, terrainSize_.y * 0.5f);
	Vector3 halfSize = Vector3(terrainSize_.x * 0.5f, 50.0f, terrainSize_.y * 0.5f);
	octree_ = std::make_unique<TerrainQuadTree>(AABB{ center, halfSize });

	/// Octreeに頂点を登録
	for (auto& vertex : vertices_) {
		octree_->Insert(&vertex);
	}


	/// 頂点のinputを行う
	//InputVertices();
}

void Terrain::Update() {
	/*for (auto& chunk : chunks_) {


		/// チャンクの描画
		Vector3 chunkSize3 = Vector3(chunk.GetChunkSize().x, 50.0f, chunk.GetChunkSize().y);
		Gizmo::DrawWireCube(
			chunk.GetPosition() + chunkSize3 * 0.5f,
			chunkSize3,
			Color::kRed
		);
	}*/


	//octree_->Draw(octree_.get(), Color::kBlack);

	if (CustomMeshRenderer* meshRenderer = GetComponent<CustomMeshRenderer>()) {
		meshRenderer->SetVertices(vertices_);
		//meshRenderer->SetIndices(indices_);
		//meshRenderer->SetIsBufferRecreate(true);
	}
}

bool Terrain::Collision(Transform* _transform, ToTerrainCollider* _toTerrainCollider) {
	_toTerrainCollider->SetIsCollided(false);

	/// 最近接点から地形との当たり判定を取る
	std::vector<Mesh::VertexData*> hitPoints;
	octree_->QuerySphere(
		_transform->GetPosition(), 1.0f, &hitPoints
	);

	/// 衝突している点に位置を合わせる
	if (hitPoints.size() > 0) {
		Vector3 average = Vector3::kZero;
		for (auto& point : hitPoints) {
			average += Vector3(point->position.x, point->position.y, point->position.z);
		}
		average /= static_cast<float>(hitPoints.size());
		//_transform->SetPosition(average);
		_transform->SetPositionY(average.y);
		_toTerrainCollider->SetIsCollided(true);
		return true;
	}


	return false;
}

void Terrain::InputVertices() {
	const std::string&& filePath = "Packages/Jsons/Terrain/TerrainVertices.json";

	/// Jsonから頂点を読み込む
	std::ifstream inFile(filePath);
	if (!inFile.is_open()) {
		Assert(false, ("Failed to open file for reading: " + filePath).c_str());
		return;
	}
	nlohmann::json jsonVertices;
	inFile >> jsonVertices;
	inFile.close();

	/// 頂点の読み込み
	for (size_t i = 0; i < jsonVertices.size(); ++i) {
		const auto& vertexData = jsonVertices[i];
		Mesh::VertexData vertex;
		vertex.position = Vector4(
			vertexData["position"][0].get<float>(),
			vertexData["position"][1].get<float>(),
			vertexData["position"][2].get<float>(),
			1.0f
		);
		vertex.normal = Vector3(
			vertexData["normal"][0].get<float>(),
			vertexData["normal"][1].get<float>(),
			vertexData["normal"][2].get<float>()
		);
		vertex.uv = Vector2(
			vertexData["uv"][0].get<float>(),
			vertexData["uv"][1].get<float>()
		);
		vertices_[i] = vertex;
	}

}
