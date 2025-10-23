#include "Terrain.h"

/// std
#include <array>

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Input/Input.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Asset/Collection/AssetCollection.h"


/// ///////////////////////////////////////////////////
/// 地形のComponentのデバッグ用関数
/// ///////////////////////////////////////////////////

void COMP_DEBUG::TerrainDebug(Terrain* _terrain, EntityComponentSystem* _ecs, AssetCollection* _assetCollection) {
	if (!_terrain) {
		return;
	}

	//float inputFloat = 0.0f;
	//if (Input::PressKey(DIK_LSHIFT)) {
	//	inputFloat = Input::GetMouseWheel() * 0.025f;
	//}

	//_terrain->brushRadius_ += inputFloat;

	/// ---------------------------------------------------
	/// 地形の編集モードを切り替え
	/// ---------------------------------------------------

	/// Buttonで切り替え

	/// ボタンの数だけテクスチャを用意
	const size_t kMaxButtonNum = static_cast<size_t>(Terrain::EditMode::Count);
	std::array<Texture*, kMaxButtonNum> buttonTextures;
	buttonTextures[static_cast<size_t>(Terrain::EditMode::None)] = _assetCollection->GetTexture("./Packages/Textures/ImGui/TerrainEditTextures/BrushModeIcon.png");
	buttonTextures[static_cast<size_t>(Terrain::EditMode::Vertex)] = _assetCollection->GetTexture("./Packages/Textures/ImGui/TerrainEditTextures/BrushModeIcon.png");
	buttonTextures[static_cast<size_t>(Terrain::EditMode::Texture)] = _assetCollection->GetTexture("./Packages/Textures/ImGui/TerrainEditTextures/BrushModeIcon.png");

	/// ボタンの説明文
	const std::array<const char*, kMaxButtonNum> descriptions = {
		"Edit Mode: 操作無し\nNo terrain editing.",
		"Edit Mode: 地形の勾配操作\nEdit terrain vertex heights.",
		"Edit Mode: テクスチャペイント\nPaint terrain textures."
	};

	/// 要素ごとにボタンを表示
	for (size_t i = 0; i < kMaxButtonNum; i++) {
		ImGui::PushID(i);

		/// 最初以外が一行になるようにする
		if (i != 0) { ImGui::SameLine(); }

		ImTextureID textureId = reinterpret_cast<ImTextureID>(buttonTextures[i]->GetSRVGPUHandle().ptr);
		if (ImGui::ImageButton("##Button", textureId, ImVec2(32, 32))) {
			_terrain->editMode_ = static_cast<int32_t>(i);
		}

		/// ボタンにカーソルを2秒以上合わせると説明を表示
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text(descriptions[i]);
			ImGui::EndTooltip();
		}

		ImGui::PopID();
	}


	/// ---------------------------------------------------
	/// Modeごとの編集内容を表示
	/// ---------------------------------------------------

	ImGui::Separator();
	switch (_terrain->editMode_) {
	case static_cast<int32_t>(Terrain::EditMode::Vertex):

		break;
	case static_cast<int32_t>(Terrain::EditMode::Texture):
		TerrainTextureEditModeDebug(&_terrain->splattingTexPaths_, _terrain->usedTextureIndex_, _assetCollection);
		break;
	}
	ImGui::Separator();



	/// ---------------------------------------------------
	/// brush の情報を変更
	/// ---------------------------------------------------

	/// brush の情報を変更
	ImGui::SliderFloat("brush radius", &_terrain->brushRadius_, 0.1f, 100.0f);
	ImGui::SliderFloat("brush strength", &_terrain->brushStrength_, 0.0f, 5.0f);



	/// flags
	ImGui::Checkbox("is rendering procedural", &_terrain->isRenderingProcedural_);

	/// river
	_terrain->river_.Edit(_ecs);
}

bool COMP_DEBUG::TerrainTextureEditModeDebug(std::array<std::string, 4>* _texturePaths, int32_t _usedTextureIndex, AssetCollection* _assetCollection) {
	/// ----- テクスチャのパスを変更する処理 ----- ///

	for (size_t i = 0; i < 4; i++) {
		std::string& text = (*_texturePaths)[i];

		ImGui::PushID((*_texturePaths)[i].c_str());


		Texture* texture = _assetCollection->GetTexture(text);
		if (texture) {

			/// 地形に使用しているテクスチャを表示
			ImTextureID id = reinterpret_cast<ImTextureID>(texture->GetSRVGPUHandle().ptr);
			ImGui::Image(id, ImVec2(48, 48));

			/// ドロップしてテクスチャを変える
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
					if (payload->Data) {
						const char* droppedPath = static_cast<const char*>(payload->Data);
						std::string path = std::string(droppedPath);

						/// パスの拡張子をチェック
						const std::string extension = Mathf::FileExtension(path);
						if (CheckAssetType(extension, AssetType::Texture)) {

							text = path;
						} else {
							Console::LogError("Invalid entity format. Please use \".prefab\"");
						}
					}
				}

				ImGui::EndDragDropTarget();
			}



			/// チェックボックスで現在操作しているのか可視化する



		}



		ImGui::PopID();
	}



	/// 変更検出、returnする


	return false;
}


void from_json(const nlohmann::json& _j, Terrain& _t) {
	_t.enable = _j.value("enable", 1);
	_t.brushRadius_ = _j.value("brushRadius", 10.0f);
	_t.brushStrength_ = _j.value("brushStrength", 1.0f);
	_t.isRenderingProcedural_ = _j.value("isRenderingProcedural", false);
}

void to_json(nlohmann::json& _j, const Terrain& _t) {
	_j = nlohmann::json{
		{ "type", "Terrain" },
		{ "enable", _t.enable },
		{ "brushRadius", _t.brushRadius_ },
		{ "brushStrength", _t.brushStrength_ },
		{ "isRenderingProcedural", _t.isRenderingProcedural_ }
	};
}




/// ///////////////////////////////////////////////////
/// 地形のComponent
/// ///////////////////////////////////////////////////


Terrain::Terrain() {

	isCreated_ = false;

	/// 頂点の生成
	const size_t terrainWidth = static_cast<size_t>(terrainSize_.x);
	const size_t terrainHeight = static_cast<size_t>(terrainSize_.y);

	/// 頂点の数
	maxVertexNum_ = static_cast<uint32_t>(terrainWidth * terrainHeight);
	const size_t faceVerts = 6;
	maxIndexNum_ = static_cast<uint32_t>((terrainWidth - 1) * (terrainHeight - 1) * faceVerts);

	/// river


	splattingTexPaths_[GRASS] = "./Packages/Textures/Grass.jpg";
	splattingTexPaths_[DIRT] = "./Packages/Textures/Dirt.jpg";
	splattingTexPaths_[ROCK] = "./Packages/Textures/Rock.jpg";
	splattingTexPaths_[SNOW] = "./Packages/Textures/Snow.jpg";

	/*---- - flags---- - */
	isRenderingProcedural_ = false;


	/// ----- editor parameters ----- ///
	brushRadius_ = 10.0f;
	brushStrength_ = 1.0f;
	editMode_ = static_cast<int32_t>(EditMode::None);

}
Terrain::~Terrain() {}

const std::array<std::string, kMaxTerrainTextureNum>& Terrain::GetSplatTexPaths() const {
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

River* Terrain::GetRiver() {
	return &river_;
}

bool Terrain::GetIsRenderingProcedural() const {
	return isRenderingProcedural_;
}

void Terrain::SetIsRenderingProcedural(bool _isRenderingProcedural) {
	isRenderingProcedural_ = _isRenderingProcedural;
}


