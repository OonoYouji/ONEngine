#include "VoxelTerrain.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/AssetDebugger.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"

void COMP_DEBUG::VoxelTerrainDebug(VoxelTerrain* _voxelTerrain) {
	if (!_voxelTerrain) {
		Console::LogError("VoxelTerrainDebug: _voxelTerrain is nullptr");
		return;
	}

	/// チャンクのデバッグ表示
	ImMathf::DragInt2("Chunk Count XZ", &_voxelTerrain->chunkCountXZ_, 1, 1, 32);
	ImMathf::DragInt3("Chunk Size", &_voxelTerrain->chunkSize_, 1, 1, 1024);
	ImMathf::DragInt3("Texture Size", &_voxelTerrain->textureSize_, 1, 1, 256);

	ImMathf::MaterialEdit("Material", &_voxelTerrain->material_, nullptr, false);


	/// 仮
	if (ImGui::Button("Create Texture3D (all chunks)")) {
		for (size_t i = 0; i < _voxelTerrain->maxChunkCount_; i++) {
			const std::wstring filename = L"./Packages/Textures/Terrain/Chunk/" + std::to_wstring(i) + L".dds";
			SaveTextureToDDS(
				filename,
				_voxelTerrain->textureSize_.x,
				_voxelTerrain->textureSize_.y,
				_voxelTerrain->textureSize_.z,
				true
			);
		}
	}


	/// ----- Gizmoでチャンクの枠線を描画 ----- ///
	static bool showChunkBounds = false;
	ImMathf::Checkbox("Show Chunk Bounds", &showChunkBounds);
	if (showChunkBounds) {
		const Vector3Int& chunkSizeInt = _voxelTerrain->GetChunkSize();
		const Vector2Int& chunkCount = _voxelTerrain->GetChunkCountXZ();
		for (int x = 0; x < chunkCount.x; ++x) {
			for (int z = 0; z < chunkCount.y; ++z) {
				// 各チャンクの位置を計算
				Vector3 chunkSize = Vector3(
					static_cast<float>(chunkSizeInt.x),
					static_cast<float>(chunkSizeInt.y),
					static_cast<float>(chunkSizeInt.z)
				);

				Vector3 chunkPosition = Vector3(static_cast<float>(x), 0.0f, static_cast<float>(z)) * chunkSize;
				chunkPosition += chunkSize * 0.5f;
				chunkPosition -= Vector3(0.5f, 0.0f, 0.5f); // 中心を合わせる調整

				// Gizmoで枠線を描画
				Gizmo::DrawWireCube(chunkPosition, chunkSize, Color::kWhite);
			}
		}
	}



}


void from_json(const nlohmann::json& _j, VoxelTerrain& _voxelTerrain) {
	/// Json -> VoxelTerrain
	_voxelTerrain.enable = _j.value("enable", 1);

	_voxelTerrain.maxChunkCount_ = _j.value("maxChunkCount", 400);
	_voxelTerrain.chunkCountXZ_ = _j.value("chunkCountXZ", Vector2Int{ 2, 2 });
	_voxelTerrain.chunkSize_ = _j.value("chunkSize", Vector3Int{ 16, 128, 16 });
	_voxelTerrain.textureSize_ = _j.value("textureSize", Vector3Int{ 32, 32, 32 });

	_voxelTerrain.material_ = _j.value("material", Material{});
}

void to_json(nlohmann::json& _j, const VoxelTerrain& _voxelTerrain) {
	/// VoxelTerrain -> Json
	_j = {
		{ "type", "VoxelTerrain" },
		{ "enable", _voxelTerrain.enable },
		{ "maxChunkCount", _voxelTerrain.maxChunkCount_ },
		{ "chunkSize", _voxelTerrain.chunkSize_ },
		{ "textureSize", _voxelTerrain.textureSize_ },
		{ "chunkCountXZ", _voxelTerrain.chunkCountXZ_ },
		{ "material", _voxelTerrain.material_ },
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
	cBufferMaterial_.Create(_dxDevice);
}

void VoxelTerrain::SetupGraphicBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 3> _rootParamIndices, AssetCollection* _assetCollection) {
	maxChunkCount_ = static_cast<UINT>(chunkCountXZ_.x * chunkCountXZ_.y);

	/// VoxelTerrainInfoの設定
	Vector3 terrainOrigin = GetOwner()->GetTransform()->GetPosition();
	cBufferTerrainInfo_.SetMappedData(GPUData::VoxelTerrainInfo{ terrainOrigin, 0, textureSize_, 0, chunkSize_, 0, chunkCountXZ_, maxChunkCount_ });
	cBufferTerrainInfo_.BindForGraphicsCommandList(_cmdList, _rootParamIndices[0]);

	/// Materialの設定
	cBufferMaterial_.SetMappedData({
		.baseColor = material_.baseColor,
		.postEffectFlags = material_.postEffectFlags,
		.entityId = GetOwner()->GetId(),
		});
	cBufferMaterial_.BindForGraphicsCommandList(_cmdList, _rootParamIndices[1]);

	/// ChunkArrayの設定
	for (size_t i = 0; i < maxChunkCount_; i++) {
		int32_t textureIndex = _assetCollection->GetTextureIndexFromGuid(chunks_[i].texture3DId);

		/// 0番は必ず存在する想定なので、見つからなかったら0番を設定する
		if (textureIndex == -1) {
			textureIndex = _assetCollection->GetTextureIndexFromGuid(chunks_[0].texture3DId);
		}

		sBufferChunks_.SetMappedData(i, GPUData::Chunk{ static_cast<uint32_t>(textureIndex) });
	}

	sBufferChunks_.SRVBindForGraphicsCommandList(_cmdList, _rootParamIndices[2]);
}

void VoxelTerrain::TransitionTextureStates(DxCommand* _dxCommand, AssetCollection* _assetCollection, D3D12_RESOURCE_STATES _beforeState, D3D12_RESOURCE_STATES _afterState) {
	/// チャンク用テクスチャの状態遷移
	for (size_t i = 0; i < maxChunkCount_; i++) {
		const std::string filepath = "./Packages/Textures/Terrain/Chunk/" + std::to_string(i) + ".dds";

		if(Texture* texture = _assetCollection->GetTexture(filepath)) {
			texture->GetDxResource().CreateBarrier(_beforeState, _afterState, _dxCommand);
		}
	}
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

bool VoxelTerrain::CheckBufferCreatedForEditor() const {
	bool result = false;

	result |= cBufferInputInfo_.Get() != nullptr;
	result |= cBufferEditInfo_.Get() != nullptr;

	return result;
}

void VoxelTerrain::CreateEditorBuffers(DxDevice* _dxDevice) {
	cBufferInputInfo_.Create(_dxDevice);
	cBufferEditInfo_.Create(_dxDevice);
}

void VoxelTerrain::SetupEditorBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 3> _rootParamIndices, const GPUData::InputInfo& _inputInfo, const GPUData::EditInfo& _editInfo) {
	/// InputInfoの設定
	cBufferInputInfo_.SetMappedData(_inputInfo);
	cBufferInputInfo_.BindForComputeCommandList(_cmdList, _rootParamIndices[0]);
	/// EditInfoの設定
	cBufferEditInfo_.SetMappedData(_editInfo);
	cBufferEditInfo_.BindForComputeCommandList(_cmdList, _rootParamIndices[1]);
	/// Chunk
	sBufferChunks_.SRVBindForComputeCommandList(_cmdList, _rootParamIndices[2]);
}

void VoxelTerrain::CreateChunkTextureUAV(DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap, AssetCollection* _assetCollection) {

	for (size_t i = 0; i < maxChunkCount_; i++) {
		const std::string filepath = "./Packages/Textures/Terrain/Chunk/" + std::to_string(i) + ".dds";

		/// 存在するテクスチャのみUAVを作成
		if(Texture* texture = _assetCollection->GetTexture(filepath)) {

			const Vector2& texSize = texture->GetTextureSize();

			texture->CreateUAVTexture3D(
				static_cast<UINT>(texSize.x),
				static_cast<UINT>(texSize.y),
				static_cast<UINT>(texSize.x),
				_dxDevice,
				_dxCommand,
				_dxSRVHeap,
				DXGI_FORMAT_R8G8B8A8_TYPELESS
			);
		}

	}

}
