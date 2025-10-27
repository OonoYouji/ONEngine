#include "Terrain.h"

/// std
#include <array>

/// externals
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

/// engine
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Utility/Input/Input.h"
#include "Engine/Core/ImGui/ImGuiSelection.h"


/// ///////////////////////////////////////////////////
/// 地形のComponentのデバッグ用関数
/// ///////////////////////////////////////////////////

void COMP_DEBUG::TerrainDebug(Terrain* _terrain, EntityComponentSystem* _ecs, AssetCollection* _assetCollection) {
	if (!_terrain) {
		return;
	}

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
		"Edit Mode: 操作無し\nNo terrain editing.\n ショートカットキー [Ctrl+N]",
		"Edit Mode: 地形の勾配操作\nEdit terrain vertex heights.\n ショートカットキー [Ctrl+V]",
		"Edit Mode: テクスチャペイント\nPaint terrain textures.\n ショートカットキー [Ctrl+B]"
	};

	/// 要素ごとにボタンを表示
	for (size_t i = 0; i < kMaxButtonNum; i++) {
		ImGui::PushID(static_cast<int>(i));

		/// 最初以外が一行になるようにする
		if (i != 0) { ImGui::SameLine(); }

		/// 非選択時は半透明にする
		int32_t saveEditMode = _terrain->editorInfo_.editMode;
		if (saveEditMode != static_cast<int32_t>(i)) {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.2f);
		}

		ImTextureID textureId = reinterpret_cast<ImTextureID>(buttonTextures[i]->GetSRVGPUHandle().ptr);
		if (ImGui::ImageButton("##Button", textureId, ImVec2(32, 32))) {
			_terrain->editorInfo_.editMode = static_cast<int32_t>(i);
		}

		/// 非選択時はスタイルを戻す
		if (saveEditMode != static_cast<int32_t>(i)) {
			ImGui::PopStyleVar();
		}

		/// ボタンにカーソルを2秒以上合わせると説明を表示
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text(descriptions[i]);
			ImGui::EndTooltip();
		}

		ImGui::PopID();
	}


	/// 編集モードの変更
	if (Input::PressKey(DIK_LCONTROL) && !Input::PressKey(DIK_LSHIFT)) {
		if (Input::TriggerKey(DIK_N)) { _terrain->editorInfo_.editMode = static_cast<int32_t>(Terrain::EditMode::None); }
		if (Input::TriggerKey(DIK_V)) { _terrain->editorInfo_.editMode = static_cast<int32_t>(Terrain::EditMode::Vertex); }
		if (Input::TriggerKey(DIK_B)) { _terrain->editorInfo_.editMode = static_cast<int32_t>(Terrain::EditMode::Texture); }
	}


	/// ---------------------------------------------------
	/// Modeごとの編集内容を表示
	/// ---------------------------------------------------

	const std::string enumStr = std::string(magic_enum::enum_name(static_cast<Terrain::EditMode>(_terrain->editorInfo_.editMode)));
	ImGui::SeparatorText(enumStr.c_str());
	switch (_terrain->editorInfo_.editMode) {
	case static_cast<int32_t>(Terrain::EditMode::Vertex):

		break;
	case static_cast<int32_t>(Terrain::EditMode::Texture):

		/// 編集するテクスチャのインデックスの変更
		if (Input::TriggerKey(DIK_1)) { _terrain->editorInfo_.usedTextureIndex = 0; }
		if (Input::TriggerKey(DIK_2)) { _terrain->editorInfo_.usedTextureIndex = 1; }
		if (Input::TriggerKey(DIK_3)) { _terrain->editorInfo_.usedTextureIndex = 2; }
		if (Input::TriggerKey(DIK_4)) { _terrain->editorInfo_.usedTextureIndex = 3; }

		TerrainTextureEditModeDebug(
			&_terrain->splattingTexPaths_,
			&_terrain->editorInfo_.usedTextureIndex,
			_assetCollection
		);
		break;
	}


	{	/// ----- 現在のエディタの情報をImGuiInfoに設定する ----- ///
		std::string info = "Terrain Editor Info :   ";
		info += "edit mode: " + enumStr;
		if(_terrain->editorInfo_.editMode == static_cast<int32_t>(Terrain::EditMode::Texture)) {
			info += "   :   used texture index: " + std::to_string(_terrain->editorInfo_.usedTextureIndex);
		}

		ImGuiInfo::SetInfo(info);
	}


	/// ---------------------------------------------------
	/// brush の情報を変更
	/// ---------------------------------------------------
	ImGui::SeparatorText("Brush data");

	/// brush の情報を変更
	ImGui::SliderFloat("brush radius", &_terrain->editorInfo_.brushRadius, 0.1f, 100.0f);
	ImGui::SliderFloat("brush strength", &_terrain->editorInfo_.brushStrength, 0.0f, 5.0f);



	/// flags
	ImGui::Checkbox("is rendering procedural", &_terrain->isRenderingProcedural_);

	/// river
	_terrain->river_.Edit(_ecs);
}

bool COMP_DEBUG::TerrainTextureEditModeDebug(std::array<std::string, 4>* _texturePaths, int32_t* _usedTextureIndex, AssetCollection* _assetCollection) {
	/// ----- テクスチャのパスを変更する処理 ----- ///

	const std::vector<std::string> shortcutKeys = {
		"[Cntrl + 1]", "[Cntrl + 2]",
		"[Cntrl + 3]", "[Cntrl + 4]"
	};


	for (size_t i = 0; i < 4; i++) {
		std::string& text = (*_texturePaths)[i];

		ImGui::PushID((*_texturePaths)[i].c_str());


		Texture* texture = _assetCollection->GetTexture(text);
		if (texture) {

			/// このテクスチャを使用しているのかどうか
			bool isUsing = (i == static_cast<size_t>(*_usedTextureIndex));
			bool isHovered = false;

			/// 地形に使用しているテクスチャを表示
			ImTextureID id = reinterpret_cast<ImTextureID>(texture->GetSRVGPUHandle().ptr);
			if (ImGui::ImageButton("##imageButton", id, ImVec2(48, 48))) {
				*_usedTextureIndex = static_cast<int32_t>(i);
			}

			isHovered |= ImGui::IsItemHovered();

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


			ImGui::SameLine();


			/// チェックボックスで現在操作しているのか可視化する
			if (ImGui::Checkbox("##using", &isUsing)) {
				*_usedTextureIndex = static_cast<int32_t>(i);
			}

			isHovered |= ImGui::IsItemHovered();

			/// Hoverで説明を表示
			if (isHovered) {
				ImGui::BeginTooltip();
				ImGui::Text("Texture %zu\nPath: %s\nClick the image to select this texture for painting.", i + 1, text.c_str());
				ImGui::Text(("ショートカットキー " + shortcutKeys[i]).c_str());
				ImGui::EndTooltip();
			}

		}



		ImGui::PopID();
	}



	/// 変更検出、returnする


	return false;
}


void from_json(const nlohmann::json& _j, Terrain& _t) {
	_t.enable = _j.value("enable", 1);
	_t.editorInfo_.brushRadius = _j.value("brushRadius", 10.0f);
	_t.editorInfo_.brushStrength = _j.value("brushStrength", 1.0f);
	_t.isRenderingProcedural_ = _j.value("isRenderingProcedural", false);
}

void to_json(nlohmann::json& _j, const Terrain& _t) {
	_j = nlohmann::json{
		{ "type", "Terrain" },
		{ "enable", _t.enable },
		{ "brushRadius", _t.editorInfo_.brushRadius },
		{ "brushStrength", _t.editorInfo_.brushStrength },
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



	splattingTexPaths_[GRASS] = "./Packages/Textures/Grass.jpg";
	splattingTexPaths_[DIRT] = "./Packages/Textures/Dirt.jpg";
	splattingTexPaths_[ROCK] = "./Packages/Textures/Rock.jpg";
	splattingTexPaths_[SNOW] = "./Packages/Textures/Snow.jpg";

	/*---- - flags---- - */
	isRenderingProcedural_ = false;


	/// ----- editor parameters ----- ///
	editorInfo_.brushRadius = 10.0f;
	editorInfo_.brushStrength = 1.0f;
	editorInfo_.editMode = static_cast<int32_t>(EditMode::None);
	editorInfo_.usedTextureIndex = 0;

}

Terrain::~Terrain() {}



void Terrain::CreateVerticesAndIndicesBuffers(DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSrvHeap) {
	/// ----- UAV buffer の作成 ----- ///
	rwVertices_.CreateUAV(GetMaxVertexNum(), _dxDevice, _dxCommand, _dxSrvHeap);
	rwIndices_.CreateUAV(GetMaxIndexNum(), _dxDevice, _dxCommand, _dxSrvHeap);
}

void Terrain::CreateRenderingBarriers(DxCommand* _dxCommand) {
	rwVertices_.GetResource().CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		_dxCommand
	);

	rwIndices_.GetResource().CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		_dxCommand
	);
}

void Terrain::RestoreResourceBarriers(DxCommand* _dxCommand) {
	rwVertices_.GetResource().CreateBarrier(
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		_dxCommand
	);

	rwIndices_.GetResource().CreateBarrier(
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		_dxCommand
	);
}

D3D12_VERTEX_BUFFER_VIEW Terrain::CreateVBV() {
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = rwVertices_.GetResource().Get()->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(TerrainVertex);
	vbv.SizeInBytes = sizeof(TerrainVertex) * GetMaxVertexNum();
	return vbv;
}

D3D12_INDEX_BUFFER_VIEW Terrain::CreateIBV() {
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = rwIndices_.GetResource().Get()->GetGPUVirtualAddress();
	ibv.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * GetMaxIndexNum());
	ibv.Format = DXGI_FORMAT_R32_UINT;
	return ibv;
}

const std::array<std::string, kMaxTerrainTextureNum>& Terrain::GetSplatTexPaths() const {
	return splattingTexPaths_;
}

const StructuredBuffer<TerrainVertex>& Terrain::GetRwVertices() const {
	return rwVertices_;
}

const StructuredBuffer<uint32_t>& Terrain::GetRwIndices() const {
	return rwIndices_;
}

DxResource& Terrain::GetVerticesResource() {
	return rwVertices_.GetResource();
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

const TerrainEditorInfo& Terrain::GetEditorInfo() const {
	return editorInfo_;
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


