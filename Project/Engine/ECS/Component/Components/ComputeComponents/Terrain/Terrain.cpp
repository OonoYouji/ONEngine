#include "Terrain.h"

/// externals
#include <imgui.h>

Terrain::Terrain() {

	isCreated_ = false;

	/// 頂点の生成
	const size_t terrainWidth = static_cast<size_t>(terrainSize_.x);
	const size_t terrainHeight = static_cast<size_t>(terrainSize_.y);

	/// 頂点の数
	maxVertexNum_ = static_cast<uint32_t>(terrainWidth * terrainHeight);
	const size_t faceVerts = 6;
	maxIndexNum_ = static_cast<uint32_t>((terrainWidth - 1) * (terrainHeight - 1) * faceVerts);


	/// インデックスの生成
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


	splattingTexPaths_[GRASS] = "./Packages/Textures/Grass.jpg";
	splattingTexPaths_[DIRT] = "./Packages/Textures/Dirt.jpg";
	splattingTexPaths_[ROCK] = "./Packages/Textures/Rock.jpg";
	splattingTexPaths_[SNOW] = "./Packages/Textures/Snow.jpg";


	brushRadius_ = 10.0f;
	brushStrength_ = 1.0f;

	/*---- - flags---- - */
	isRenderingProcedural_ = false;

}
Terrain::~Terrain() {}

const std::vector<uint32_t>& Terrain::GetIndices() const {
	return indices_;
}

const std::array<std::string, Terrain::SPLAT_TEX_COUNT>& Terrain::GetSplatTexPaths() const {
	return splattingTexPaths_;
}

StructuredBuffer<TerrainVertex>& Terrain::GetRwVertices() {
	return rwVertices_;
}

StructuredBuffer<uint32_t>& Terrain::GetRwIndices() {
	return rwIndices_;
}

void Terrain::SetIsCreated(bool _isCreated) {
	isCreated_ = _isCreated;
}

bool Terrain::GetIsCreated() const {
	return isCreated_;
}

uint32_t Terrain::GetMaxVertexNum() {
	return maxVertexNum_;
}

uint32_t Terrain::GetMaxIndexNum() {
	return maxIndexNum_;
}

const Vector2& Terrain::GetSize() const {
	return terrainSize_;
}

float Terrain::GetBrushRadius() const {
	return brushRadius_;
}

void Terrain::SetBrushRadius(float _radius) {
	brushRadius_ = _radius;
}

float Terrain::GetBrushStrength() const {
	return brushStrength_;
}

void Terrain::SetBrushStrength(float _strength) {
	brushStrength_ = _strength;
}

bool Terrain::GetIsRenderingProcedural() const {
	return isRenderingProcedural_;
}

void Terrain::SetIsRenderingProcedural(bool _isRenderingProcedural) {
	isRenderingProcedural_ = _isRenderingProcedural;
}



void COMP_DEBUG::TerrainDebug(Terrain* _terrain) {
	if (!_terrain) {
		return;
	}

	float brushRadius = _terrain->GetBrushRadius();
	float brushStrength = _terrain->GetBrushStrength();

	if (ImGui::SliderFloat("brush radius", &brushRadius, 0.1f, 100.0f)) {
		_terrain->SetBrushRadius(brushRadius);
	}

	if (ImGui::SliderFloat("brush strength", &brushStrength, 0.0f, 5.0f)) {
		_terrain->SetBrushStrength(brushStrength);
	}

	/// flags
	bool isRenderingProcedural = _terrain->GetIsRenderingProcedural();
	if (ImGui::Checkbox("is rendering procedural", &isRenderingProcedural)) {
		_terrain->SetIsRenderingProcedural(isRenderingProcedural);
	}

}

void from_json(const nlohmann::json& _j, Terrain& _t) {
	_t.enable = _j.value("enable", 1);
	_t.SetBrushRadius(_j.value("brushRadius", 10.0f));
	_t.SetBrushStrength(_j.value("brushStrength", 1.0f));
	_t.SetIsRenderingProcedural(_j.value("isRenderingProcedural", false));
}

void to_json(nlohmann::json& _j, const Terrain& _t) {
	_j = nlohmann::json{
		{ "type", "Terrain" },
		{ "enable", _t.enable },
		{ "brushRadius", _t.GetBrushRadius() },
		{ "brushStrength", _t.GetBrushStrength() },
		{ "isRenderingProcedural", _t.GetIsRenderingProcedural() }
	};
}
