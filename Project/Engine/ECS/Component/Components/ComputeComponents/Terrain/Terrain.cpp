#include "Terrain.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Input/Input.h"

Terrain::Terrain() {

	isCreated_ = false;

	/// 頂点の生成
	const size_t terrainWidth = static_cast<size_t>(terrainSize_.x);
	const size_t terrainHeight = static_cast<size_t>(terrainSize_.y);

	/// 頂点の数
	maxVertexNum_ = static_cast<uint32_t>(terrainWidth * terrainHeight);
	const size_t faceVerts = 6;
	maxIndexNum_ = static_cast<uint32_t>((terrainWidth - 1) * (terrainHeight - 1) * faceVerts);


	splattingTexPaths_[GRASS] = "./Packages/Textures/Grass.jpg";
	splattingTexPaths_[DIRT] = "./Packages/Textures/Dirt.jpg";
	splattingTexPaths_[ROCK] = "./Packages/Textures/Rock.jpg";
	splattingTexPaths_[SNOW] = "./Packages/Textures/Snow.jpg";

	/*---- - flags---- - */
	isRenderingProcedural_ = false;

}
Terrain::~Terrain() {}

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

	float inputFloat = 0.0f;
	if (Input::PressKey(DIK_LSHIFT)) {
		inputFloat = Input::GetMouseWheel() * 0.025f;
	}

	_terrain->brushRadius_ += inputFloat;

	/// brush の情報を変更
	ImGui::SliderFloat("brush radius", &_terrain->brushRadius_, 0.1f, 100.0f);
	ImGui::SliderFloat("brush strength", &_terrain->brushStrength_, 0.0f, 5.0f);

	/// flags
	ImGui::Checkbox("is rendering procedural", &_terrain->isRenderingProcedural_);

}

void from_json(const nlohmann::json& _j, Terrain& _t) {
	_t.enable = _j.value("enable", 1);
	_t.brushRadius_ = _j.value("brushRadius", 10.0f);
	_t.brushStrength_ = _j.value("brushStrength", 1.0f);
	_t.isRenderingProcedural_ = _j.value("isRenderingProcedural", false);
}

void to_json(nlohmann::json& _j, const Terrain& _t) {
	_j = nlohmann::json{
		{ "type",                  "Terrain" },
		{ "enable",                _t.enable },
		{ "brushRadius",           _t.brushRadius_ },
		{ "brushStrength",         _t.brushStrength_ },
		{ "isRenderingProcedural", _t.isRenderingProcedural_ }
	};
}
