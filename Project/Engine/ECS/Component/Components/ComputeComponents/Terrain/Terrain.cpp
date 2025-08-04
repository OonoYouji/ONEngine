#include "Terrain.h"

Terrain::Terrain() {

	/// 頂点の生成
	const size_t terrainWidth = static_cast<size_t>(terrainSize_.x);
	const size_t terrainHeight = static_cast<size_t>(terrainSize_.y);
	vertices_.resize(terrainWidth * terrainHeight);

	for (size_t row = 0; row < terrainWidth; ++row) {
		for (size_t col = 0; col < terrainHeight; ++col) {
			size_t index = row * terrainWidth + col;
			vertices_[index].position = Vector4(static_cast<float>(row), 0.0f, static_cast<float>(col), 1.0f);
			vertices_[index].position -= Vector4(terrainWidth * 0.5f, 0.0f, terrainHeight * 0.5f, 0.0f);
			vertices_[index].uv = Vector2(
				static_cast<float>(row) / static_cast<float>(terrainWidth),
				static_cast<float>(col) / static_cast<float>(terrainHeight) * -1.0f
			);
			vertices_[index].normal = Vector3(0.0f, 1.0f, 0.0f);
			vertices_[index].splatBlend = Vector4(1.0f - vertices_[index].uv.x, 1.0f - vertices_[index].uv.y, 0.0f, 0.0f);
			vertices_[index].index = static_cast<int>(index); ///< インデックスを設定
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
	vertexSpan_ = std::span<std::span<TerrainVertex>>(reinterpret_cast<std::span<TerrainVertex>*>(vertices_.data()), terrainWidth);


	splattingTexPaths_[GRASS] = "./Packages/Textures/Grass.jpg";
	splattingTexPaths_[DIRT] = "./Packages/Textures/Dirt.jpg";
	splattingTexPaths_[ROCK] = "./Packages/Textures/Rock.jpg";
	splattingTexPaths_[SNOW] = "./Packages/Textures/Snow.jpg";

}
Terrain::~Terrain() {}

const std::span<std::span<TerrainVertex>>& Terrain::GetVertexSpan() const {
	return vertexSpan_;
}

const std::vector<TerrainVertex>& Terrain::GetVertices() const {
	return vertices_;
}

std::vector<TerrainVertex>& Terrain::GetVertices() {
	return vertices_;
}

const std::vector<uint32_t>& Terrain::GetIndices() const {
	return indices_;
}

const std::vector<std::pair<size_t, TerrainVertex*>>& Terrain::GetEditVertices() {
	return editVertices_;
}

const std::array<std::string, Terrain::SPLAT_TEX_COUNT>& Terrain::GetSplatTexPaths() const {
	return splattingTexPaths_;
}



void COMP_DEBUG::TerrainDebug(Terrain* _terrain) {
	if (!_terrain) {
		return;
	}


}

void from_json(const nlohmann::json& _j, Terrain& _t) {
	if (_j.contains("enable")) {
		_t.enable = _j.at("enable").get<int>();
	}
}

void to_json(nlohmann::json& _j, const Terrain& _t) {
	_j = nlohmann::json{
		{ "type", "Terrain" },
		{ "enable", _t.enable },
	};
}
