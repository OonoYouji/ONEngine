#include "VoxelTerrain.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"



void COMP_DEBUG::VoxelTerrainDebug(VoxelTerrain* _voxelTerrain) {
	if (!_voxelTerrain) {
		Console::LogError("VoxelTerrainDebug: _voxelTerrain is nullptr");
		return;
	}

	/// チャンクのデバッグ表示
	ImMathf::DragInt("Max Chunk Count", reinterpret_cast<int*>(&_voxelTerrain->maxChunkCount_), 1, 1, 10000);
	ImMathf::DragInt3("Chunk Size", &_voxelTerrain->chunkSize_, 1, 1, 1024);

}


void from_json(const nlohmann::json& _j, VoxelTerrain& _voxelTerrain) {
	/// Json -> VoxelTerrain
	_voxelTerrain.enable = _j.value("enable", 1);

	_voxelTerrain.maxChunkCount_ = _j.value("maxChunkCount", 400);
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
	maxChunkCount_ = 20 * 20;
	chunkSize_ = Vector3Int{ 16, 128, 16 };
}

VoxelTerrain::~VoxelTerrain() {}

bool VoxelTerrain::CheckCreatedBuffers() const {
	bool isCreated = false;

	/// BufferのリソースポインタがNullかどうかで判定
	isCreated |= cBufferTerrainInfo_.Get() != nullptr;
	isCreated |= sBufferChunks_.GetResource().Get() != nullptr;

	return isCreated;
}

void VoxelTerrain::CreateBuffers(DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap) {
	cBufferTerrainInfo_.Create(_dxDevice);
	sBufferChunks_.Create(maxChunkCount_, _dxDevice, _dxSRVHeap);
}

void VoxelTerrain::SetupGraphicBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 2> _rootParamIndices) {
	/// VoxelTerrainInfoの設定
	cBufferTerrainInfo_.SetMappedData(GPUData::VoxelTerrainInfo{ chunkSize_ });
	cBufferTerrainInfo_.BindForGraphicsCommandList(_cmdList, _rootParamIndices[0]);

	/// ChunkArrayの設定
	sBufferChunks_.SRVBindForGraphicsCommandList(_cmdList, _rootParamIndices[1]);
}