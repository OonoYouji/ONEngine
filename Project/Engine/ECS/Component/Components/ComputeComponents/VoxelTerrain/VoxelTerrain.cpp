#include "VoxelTerrain.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"
#include "Engine/Asset/Collection/AssetCollection.h"


void COMP_DEBUG::VoxelTerrainDebug(VoxelTerrain* _voxelTerrain) {
	if (!_voxelTerrain) {
		Console::LogError("VoxelTerrainDebug: _voxelTerrain is nullptr");
		return;
	}

	/// チャンクのデバッグ表示
	ImMathf::DragInt2("Chunk Count XZ", &_voxelTerrain->chunkCountXZ_, 1, 1, 10000);
	ImMathf::DragInt3("Chunk Size", &_voxelTerrain->chunkSize_, 1, 1, 1024);


	/// 仮
	if (ImGui::Button("Create Texture3D (all chunks)")) {
		for (size_t i = 0; i < _voxelTerrain->maxChunkCount_; i++) {
			const std::wstring filename = L"./Packages/Textures/Terrain/Chunk/" + std::to_wstring(i) + L".dds";
			SaveTextureToDDS(
				filename,
				_voxelTerrain->chunkSize_.x,
				_voxelTerrain->chunkSize_.y,
				_voxelTerrain->chunkSize_.z,
				true
			);
		}
	}


}


void from_json(const nlohmann::json& _j, VoxelTerrain& _voxelTerrain) {
	/// Json -> VoxelTerrain
	_voxelTerrain.enable = _j.value("enable", 1);

	_voxelTerrain.maxChunkCount_ = _j.value("maxChunkCount", 400);
	_voxelTerrain.chunkCountXZ_ = _j.value("chunkCountXZ", Vector2Int{ 20, 20 });
	_voxelTerrain.chunkSize_ = _j.value("chunkSize", Vector3Int{ 16, 128, 16 });
}

void to_json(nlohmann::json& _j, const VoxelTerrain& _voxelTerrain) {
	/// VoxelTerrain -> Json
	_j = {
		{ "type", "VoxelTerrain" },
		{ "enable", _voxelTerrain.enable },
		{ "maxChunkCount", _voxelTerrain.maxChunkCount_ },
		{ "chunkSize", _voxelTerrain.chunkSize_ },
	};
}


/// ///////////////////////////////////////////////////
/// ボクセルで表現された地形
/// ///////////////////////////////////////////////////

VoxelTerrain::VoxelTerrain() {
	/// x*z でチャンクが並ぶ想定
	chunkCountXZ_ = Vector2Int{ 10, 10 };
	chunkSize_ = Vector3Int{ 16, 128, 16 };
	maxChunkCount_ = static_cast<UINT>(chunkCountXZ_.x * chunkCountXZ_.y);
}

VoxelTerrain::~VoxelTerrain() {}

void VoxelTerrain::SettingChunksGuid(AssetCollection* _assetCollection) {
	size_t maxChunks = static_cast<size_t>(chunkCountXZ_.x * chunkCountXZ_.y);
	if(maxChunks > chunks_.size()) {
		chunks_.resize(maxChunks);
	}

	for (size_t i = 0; i < maxChunks; i++) {
		/// indexを元にファイルパスを生成
		const std::string filepath = "./Packages/Textures/Terrain/Chunk/" + std::to_string(i) + ".dds";

		/// AssetCollectionからGuidを取得して設定
		const Guid& texture3DGuid = _assetCollection->GetAssetGuidFromPath(filepath);
		chunks_[i].texture3DId = texture3DGuid;
	}
}

bool VoxelTerrain::CheckCreatedBuffers() const {
	bool isCreated = false;

	/// BufferのリソースポインタがNullかどうかで判定
	isCreated |= cBufferTerrainInfo_.Get() != nullptr;
	isCreated |= sBufferChunks_.GetResource().Get() != nullptr;

	return isCreated;
}

void VoxelTerrain::CreateBuffers(DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap) {
	maxChunkCount_ = static_cast<UINT>(chunkCountXZ_.x * chunkCountXZ_.y);

	cBufferTerrainInfo_.Create(_dxDevice);
	sBufferChunks_.Create(maxChunkCount_, _dxDevice, _dxSRVHeap);
}

void VoxelTerrain::SetupGraphicBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 2> _rootParamIndices, AssetCollection* _assetCollection) {

	maxChunkCount_ = static_cast<UINT>(chunkCountXZ_.x * chunkCountXZ_.y);

	/// VoxelTerrainInfoの設定
	cBufferTerrainInfo_.SetMappedData(GPUData::VoxelTerrainInfo{ chunkSize_, chunkCountXZ_, maxChunkCount_ });
	cBufferTerrainInfo_.BindForGraphicsCommandList(_cmdList, _rootParamIndices[0]);

	/// ChunkArrayの設定
	size_t maxChunks = static_cast<size_t>(chunkCountXZ_.x * chunkCountXZ_.y);
	for (size_t i = 0; i < maxChunks; i++) {
		uint32_t textureIndex = _assetCollection->GetTextureIndexFromGuid(chunks_[i].texture3DId);
		sBufferChunks_.SetMappedData(i, GPUData::Chunk{ textureIndex });
	}

	sBufferChunks_.SRVBindForGraphicsCommandList(_cmdList, _rootParamIndices[1]);
}