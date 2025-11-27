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
	ImMathf::DragInt2("Chunk Count XZ", &_voxelTerrain->chunkCountXZ_, 1, 1, 32);
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
	_voxelTerrain.chunkCountXZ_ = _j.value("chunkCountXZ", Vector2Int{ 2, 2 });
	_voxelTerrain.chunkSize_ = _j.value("chunkSize", Vector3Int{ 16, 128, 16 });
}

void to_json(nlohmann::json& _j, const VoxelTerrain& _voxelTerrain) {
	/// VoxelTerrain -> Json
	_j = {
		{ "type", "VoxelTerrain" },
		{ "enable", _voxelTerrain.enable },
		{ "maxChunkCount", _voxelTerrain.maxChunkCount_ },
		{ "chunkSize", _voxelTerrain.chunkSize_ },
		{ "chunkCountXZ", _voxelTerrain.chunkCountXZ_ }
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
	maxChunkCount_ = static_cast<size_t>(chunkCountXZ_.x * chunkCountXZ_.y);
	if (maxChunkCount_ > chunks_.size()) {
		chunks_.resize(maxChunkCount_);
	}

	for (size_t i = 0; i < maxChunkCount_; i++) {
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
	//maxChunkCount_ = static_cast<UINT>(chunkCountXZ_.x * chunkCountXZ_.y);
	UINT chunkCount = static_cast<UINT>(32 * 32);

	cBufferTerrainInfo_.Create(_dxDevice);
	sBufferChunks_.Create(chunkCount, _dxDevice, _dxSRVHeap);
}

void VoxelTerrain::SetupGraphicBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 2> _rootParamIndices, AssetCollection* _assetCollection) {
	maxChunkCount_ = static_cast<UINT>(chunkCountXZ_.x * chunkCountXZ_.y);

	/// VoxelTerrainInfoの設定
	cBufferTerrainInfo_.SetMappedData(GPUData::VoxelTerrainInfo{ chunkSize_, 0.0f/*pad*/, chunkCountXZ_, maxChunkCount_ });
	cBufferTerrainInfo_.BindForGraphicsCommandList(_cmdList, _rootParamIndices[0]);

	/// ChunkArrayの設定
	for (size_t i = 0; i < maxChunkCount_; i++) {
		int32_t textureIndex = _assetCollection->GetTextureIndexFromGuid(chunks_[i].texture3DId);

		/// 0番は必ず存在する想定なので、見つからなかったら0番を設定する
		if (textureIndex == -1) {
			textureIndex = _assetCollection->GetTextureIndexFromGuid(chunks_[0].texture3DId);
		}

		sBufferChunks_.SetMappedData(i, GPUData::Chunk{ static_cast<uint32_t>(textureIndex) });
	}

	sBufferChunks_.SRVBindForGraphicsCommandList(_cmdList, _rootParamIndices[1]);
}

UINT VoxelTerrain::MaxChunkCount() const {
	return maxChunkCount_;
}

const Vector2Int& VoxelTerrain::GetChunkCountXZ() const {
	return chunkCountXZ_;
}

const Vector3Int& VoxelTerrain::GetChunkSize() const {
	return chunkSize_;
}
