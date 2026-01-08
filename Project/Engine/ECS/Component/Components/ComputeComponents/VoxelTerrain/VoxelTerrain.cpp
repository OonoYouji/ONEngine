#include "VoxelTerrain.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"

/// editor
#include "Engine/Editor/Math/AssetDebugger.h"

using namespace ONEngine;

void ComponentDebug::VoxelTerrainDebug(VoxelTerrain* _voxelTerrain, DxManager* _dxm) {
	if(!_voxelTerrain) {
		Console::LogError("VoxelTerrainDebug: _voxelTerrain is nullptr");
		return;
	}


	Editor::ImMathf::Checkbox("Can MeshShader Rendering", &_voxelTerrain->canMeshShaderRendering_);
	Editor::ImMathf::Checkbox("Can VertexShader Rendering", &_voxelTerrain->canVertexShaderRendering_);

	ImGui::Separator();

	/// チャンクのデバッグ表示
	Editor::ImMathf::DragInt2("Chunk Count XZ", &_voxelTerrain->chunkCountXZ_, 1, 1, 32);
	Editor::ImMathf::DragInt3("Chunk Size", &_voxelTerrain->chunkSize_, 1, 1, 1024);
	Editor::ImMathf::DragInt3("Texture Size", &_voxelTerrain->textureSize_, 1, 1, 256);
	/// テクスチャサイズを変更したらpTextureに適用する
	if(ImGui::Button("Texture Size Apply And Output")) {
		for(auto& chunk : _voxelTerrain->chunks_) {
			if(chunk.pTexture) {
				chunk.pTexture->ResizeTexture3D(
					Vector2(static_cast<float>(_voxelTerrain->textureSize_.x), static_cast<float>(_voxelTerrain->textureSize_.y)),
					static_cast<UINT>(_voxelTerrain->textureSize_.z),
					_dxm->GetDxDevice(),
					_dxm->GetDxCommand(),
					_dxm->GetDxSRVHeap()
				);
			}
		}

		_dxm->HeapBindToCommandList();

		std::wstring filepath = L"";
		for(size_t i = 0; i < _voxelTerrain->chunks_.size(); i++) {
			filepath = L"./Packages/Textures/Terrain/Chunk/" + std::to_wstring(i) + L".dds";

			const Chunk& chunk = _voxelTerrain->chunks_[i];
			chunk.pTexture->OutputTexture3D(filepath, _dxm->GetDxDevice(), _dxm->GetDxCommand());
			Console::Log("Chunk " + std::to_string(i) + ": Texture3D GUID = " + chunk.texture3DId.ToString());
		}

		_dxm->HeapBindToCommandList();
	}

	Editor::ImMathf::MaterialEdit("Material", &_voxelTerrain->material_, nullptr, false);

	/// editor用
	{
		static float radius = 5.0f;
		Editor::ImMathf::DragFloat("Brush Radius", &radius, 0.1f, 1.0f, 100.0f);
		_voxelTerrain->cBufferEditInfo_.SetMappedData({ radius });
	}



	/// 仮
	if(ImGui::Button("Create Texture3D (all chunks)")) {
		for(size_t i = 0; i < _voxelTerrain->maxChunkCount_; i++) {
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

	/// 出力用
	if(ImGui::Button("Output Chunk Textures Info")) {
		std::wstring filepath = L"";
		for(size_t i = 0; i < _voxelTerrain->chunks_.size(); i++) {
			filepath = L"./Packages/Textures/Terrain/Chunk/" + std::to_wstring(i) + L".dds";

			const Chunk& chunk = _voxelTerrain->chunks_[i];
			chunk.pTexture->OutputTexture3D(filepath, _dxm->GetDxDevice(), _dxm->GetDxCommand());
			Console::Log("Chunk " + std::to_string(i) + ": Texture3D GUID = " + chunk.texture3DId.ToString());
		}
	}


	/// ----- Gizmoでチャンクの枠線を描画 ----- ///
	Editor::ImMathf::Checkbox("IsEditMode", &_voxelTerrain->isEditMode_);
	static bool showChunkBounds = false;
	Editor::ImMathf::Checkbox("Show Chunk Bounds", &showChunkBounds);
	if(showChunkBounds) {
		const Vector3Int& chunkSizeInt = _voxelTerrain->GetChunkSize();
		const Vector2Int& chunkCount = _voxelTerrain->GetChunkCountXZ();
		for(int x = 0; x < chunkCount.x; ++x) {
			for(int z = 0; z < chunkCount.y; ++z) {
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


void ONEngine::from_json(const nlohmann::json& _j, std::vector<Chunk>& _chunks) {
	nlohmann::json jChunks = _j["chunks"];

	_chunks.resize(jChunks.size());

	std::string key;
	for(size_t i = 0; i < jChunks.size(); i++) {
		key = std::to_string(i);
		if(jChunks.contains(key)) {
			_chunks[i] = Chunk{ jChunks[key], nullptr };
		}
	}
}

void ONEngine::to_json(nlohmann::json& _j, const std::vector<Chunk>& _chunks) {
	nlohmann::json jChunks;

	for(size_t i = 0; i < _chunks.size(); i++) {
		jChunks[std::to_string(i)] = _chunks[i].texture3DId;
	}

	_j = {
		{ "chunks", jChunks }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, VoxelTerrain& _voxelTerrain) {
	/// Json -> VoxelTerrain
	_voxelTerrain.enable = _j.value("enable", 1);

	_voxelTerrain.maxChunkCount_ = _j.value("maxChunkCount", 400);
	_voxelTerrain.chunkCountXZ_ = _j.value("chunkCountXZ", Vector2Int{ 2, 2 });
	_voxelTerrain.chunkSize_ = _j.value("chunkSize", Vector3Int{ 16, 128, 16 });
	_voxelTerrain.textureSize_ = _j.value("textureSize", Vector3Int{ 32, 32, 32 });

	_voxelTerrain.material_ = _j.value("material", Material{});
	_voxelTerrain.chunks_ = _j.value("chunks", std::vector<Chunk>{});
}

void ONEngine::to_json(nlohmann::json& _j, const VoxelTerrain& _voxelTerrain) {
	/// VoxelTerrain -> Json
	_j = {
		{ "type", "VoxelTerrain" },
		{ "enable", _voxelTerrain.enable },
		{ "maxChunkCount", _voxelTerrain.maxChunkCount_ },
		{ "chunkSize", _voxelTerrain.chunkSize_ },
		{ "textureSize", _voxelTerrain.textureSize_ },
		{ "chunkCountXZ", _voxelTerrain.chunkCountXZ_ },
		{ "material", _voxelTerrain.material_ },
		{ "chunks", _voxelTerrain.chunks_ }
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

VoxelTerrain::~VoxelTerrain() {
	if(pDxSRVHeap_) {
		/// 使用しているUAVテクスチャの解放を行う
		for(auto& chunk : chunks_) {
			if(chunk.uavTexture.HasUAVHandle()) {
				pDxSRVHeap_->Free(chunk.uavTexture.GetUAVHandle().descriptorIndex);
			}
		}
	}
}

void VoxelTerrain::SettingChunksGuid(AssetCollection* _assetCollection) {
	maxChunkCount_ = static_cast<size_t>(chunkCountXZ_.x * chunkCountXZ_.y);
	if(maxChunkCount_ > chunks_.size()) {
		chunks_.resize(maxChunkCount_);
	}

	for(size_t i = 0; i < maxChunkCount_; i++) {
		/// indexを元にファイルパスを生成
		const std::string filepath = "./Packages/Textures/Terrain/Chunk/" + std::to_string(i) + ".dds";

		/// AssetCollectionからGuidを取得して設定
		const Guid& texture3DGuid = _assetCollection->GetAssetGuidFromPath(filepath);
		chunks_[i].texture3DId = texture3DGuid;

		Texture* texture = _assetCollection->GetTextureFromGuid(texture3DGuid);
		chunks_[i].pTexture = texture;
	}
}

bool VoxelTerrain::CheckCreatedBuffers() const {
	bool isCreated = false;

	/// BufferのリソースポインタがNullかどうかで判定
	isCreated |= cBufferTerrainInfo_.Get() != nullptr;
	isCreated |= sBufferChunks_.GetResource().Get() != nullptr;

	return isCreated;
}

void VoxelTerrain::CreateBuffers(DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, AssetCollection* _assetCollection) {
	UINT chunkCount = static_cast<UINT>(32 * 32);

	cBufferTerrainInfo_.Create(_dxDevice);
	sBufferChunks_.Create(chunkCount, _dxDevice, _dxSRVHeap);
	cBufferMaterial_.Create(_dxDevice);


	/// ChunkArrayの設定
	for(size_t i = 0; i < maxChunkCount_; i++) {
		const Texture* texture = _assetCollection->GetTextureFromGuid(chunks_[i].texture3DId);
		if(texture) {
			sBufferChunks_.SetMappedData(i, GPUData::Chunk{ static_cast<uint32_t>(texture->GetSRVDescriptorIndex()) });
		} else {
			const Texture* frontTex = _assetCollection->GetTextureFromGuid(chunks_[0].texture3DId);
			sBufferChunks_.SetMappedData(i, GPUData::Chunk{ static_cast<uint32_t>(frontTex->GetSRVDescriptorIndex()) });
		}
	}
}

void VoxelTerrain::SetupGraphicBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 3> _rootParamIndices, AssetCollection* _assetCollection) {
	maxChunkCount_ = static_cast<UINT>(chunkCountXZ_.x * chunkCountXZ_.y);

	/// VoxelTerrainInfoの設定
	Vector3 terrainOrigin = GetOwner()->GetTransform()->GetPosition();
	cBufferTerrainInfo_.SetMappedData(GPUData::VoxelTerrainInfo{ terrainOrigin, 0, textureSize_, 0, chunkSize_, 0, chunkCountXZ_, maxChunkCount_ });
	cBufferTerrainInfo_.BindForGraphicsCommandList(_cmdList, _rootParamIndices[0]);

	/// Materialの設定
	SettingMaterial();
	cBufferMaterial_.BindForGraphicsCommandList(_cmdList, _rootParamIndices[1]);

	/// ChunkArrayの設定
	for(size_t i = 0; i < maxChunkCount_; i++) {
		const Texture* texture = _assetCollection->GetTextureFromGuid(chunks_[i].texture3DId);
		if(texture) {
			sBufferChunks_.SetMappedData(i, GPUData::Chunk{ static_cast<uint32_t>(texture->GetSRVDescriptorIndex()) });
		} else {
			const Texture* frontTex = _assetCollection->GetTextureFromGuid(chunks_[0].texture3DId);
			sBufferChunks_.SetMappedData(i, GPUData::Chunk{ static_cast<uint32_t>(frontTex->GetSRVDescriptorIndex()) });
		}
	}

	sBufferChunks_.SRVBindForGraphicsCommandList(_cmdList, _rootParamIndices[2]);
}

void VoxelTerrain::TransitionTextureStates(DxCommand* _dxCommand, AssetCollection* _assetCollection, D3D12_RESOURCE_STATES _afterState) {
	/// チャンク用テクスチャの状態遷移
	std::vector<DxResource*> resources;
	resources.reserve(maxChunkCount_);
	for(size_t i = 0; i < maxChunkCount_; i++) {
		const Guid& guid = chunks_[i].texture3DId;
		chunks_[i].pTexture = _assetCollection->GetTextureFromGuid(guid);
		if(chunks_[i].pTexture) {
			resources.push_back(&chunks_[i].pTexture->GetDxResource());
		}
	}

	CreateBarriers(resources, _afterState, _dxCommand);
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

void ONEngine::VoxelTerrain::SettingMaterial() {
	/// Materialの設定
	cBufferMaterial_.SetMappedData({
		.baseColor = material_.baseColor,
		.postEffectFlags = material_.postEffectFlags,
		.entityId = GetOwner()->GetId(),
								   });
}

void VoxelTerrain::SettingTerrainInfo() {
	cBufferTerrainInfo_.SetMappedData(
		GPUData::VoxelTerrainInfo{
			.terrainOrigin = GetOwner()->GetTransform()->GetPosition(),
			.textureSize = textureSize_, .chunkSize = chunkSize_,
			.chunkCountXZ = chunkCountXZ_, .maxChunkCount = maxChunkCount_
		}
	);
}

bool VoxelTerrain::CheckBufferCreatedForEditor() const {
	bool result = false;

	result |= cBufferInputInfo_.Get() != nullptr;
	result |= cBufferEditInfo_.Get() != nullptr;

	return result;
}

void VoxelTerrain::CreateEditorBuffers(DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap) {
	UINT chunkCount = static_cast<UINT>(32 * 32);

	cBufferInputInfo_.Create(_dxDevice);
	cBufferEditInfo_.Create(_dxDevice);
	sBufferEditorChunks_.Create(chunkCount, _dxDevice, _dxSRVHeap);
}

void VoxelTerrain::SetupEditorBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 4> _rootParamIndices, AssetCollection* _assetCollection, const GPUData::InputInfo& _inputInfo) {
	/// InputInfoの設定
	cBufferInputInfo_.SetMappedData(_inputInfo);
	cBufferInputInfo_.BindForComputeCommandList(_cmdList, _rootParamIndices[0]);
	/// TerrainInfoの設定
	SettingTerrainInfo();
	cBufferTerrainInfo_.BindForComputeCommandList(_cmdList, _rootParamIndices[1]);
	/// EditInfoの設定
	cBufferEditInfo_.BindForComputeCommandList(_cmdList, _rootParamIndices[2]);

	/// ChunkArrayの設定
	for(size_t i = 0; i < maxChunkCount_; i++) {
		sBufferEditorChunks_.SetMappedData(i, GPUData::Chunk{ static_cast<uint32_t>(chunks_[i].uavTexture.GetUAVDescriptorIndex()) });
	}
	/// Chunk
	sBufferEditorChunks_.SRVBindForComputeCommandList(_cmdList, _rootParamIndices[3]);
}

void VoxelTerrain::CreateChunkTextureUAV(DxCommand* _dxCommand, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, AssetCollection* _assetCollection) {

	pDxSRVHeap_ = _dxSRVHeap;
	for(auto& chunk : chunks_) {
		chunk.uavTexture.CreateUAVTexture3D(
			static_cast<UINT>(textureSize_.x),
			static_cast<UINT>(textureSize_.y),
			static_cast<UINT>(textureSize_.z),
			_dxDevice, _dxSRVHeap,
			DXGI_FORMAT_R8G8B8A8_UNORM
		);

		//}

		//{
			//auto& chunk = chunks_[0];
		//const uint32_t vertexCount = 80000;
		//chunk.rwVertices.CreateUAV(vertexCount, _dxDevice, _dxCommand, _dxSRVHeap);
		//chunk.rwVertexCounter.CreateUAV(vertexCount, _dxDevice, _dxCommand, _dxSRVHeap);
		//chunk.vbv.Create(vertexCount, _dxDevice, _dxCommand);
	}


	D3D12_RESOURCE_STATES srvTextureBefore = chunks_[0].pTexture->GetDxResource().GetCurrentState();
	D3D12_RESOURCE_STATES uavTextureBefore = chunks_[0].uavTexture.GetDxResource().GetCurrentState();

	//auto cmdList = _dxCommand->GetCommandList();
	///// テクスチャの状態遷移
	//for(auto& chunk : chunks_) {
	//	chunk.pTexture->GetDxResource().CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);
	//	chunk.uavTexture.GetDxResource().CreateBarrier(D3D12_RESOURCE_STATE_COPY_DEST, _dxCommand);
	//}

	///// 実際に使用するSRVをUAVテクスチャにコピーする
	//for(auto& chunk : chunks_) {
	//	cmdList->CopyResource(
	//		chunk.uavTexture.GetDxResource().Get(),
	//		chunk.pTexture->GetDxResource().Get()
	//	);
	//}

	///// テクスチャの状態遷移
	//for(auto& chunk : chunks_) {
	//	chunk.pTexture->GetDxResource().CreateBarrier(srvTextureBefore, _dxCommand);
	//	chunk.uavTexture.GetDxResource().CreateBarrier(uavTextureBefore, _dxCommand);
	//}
}

void VoxelTerrain::CopyEditorTextureToChunkTexture(DxCommand* _dxCommand) {
	D3D12_RESOURCE_STATES srvTextureBefore = chunks_[0].pTexture->GetDxResource().GetCurrentState();
	D3D12_RESOURCE_STATES uavTextureBefore = chunks_[0].uavTexture.GetDxResource().GetCurrentState();

	auto cmdList = _dxCommand->GetCommandList();
	/// テクスチャの状態遷移
	for(auto& chunk : chunks_) {
		chunk.uavTexture.GetDxResource().CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);
		chunk.pTexture->GetDxResource().CreateBarrier(D3D12_RESOURCE_STATE_COPY_DEST, _dxCommand);
	}

	/// 実際に使用するSRVをUAVテクスチャにコピーする
	for(auto& chunk : chunks_) {
		cmdList->CopyResource(
			chunk.pTexture->GetDxResource().Get(),
			chunk.uavTexture.GetDxResource().Get()
		);
	}

	/// テクスチャの状態遷移
	for(auto& chunk : chunks_) {
		chunk.uavTexture.GetDxResource().CreateBarrier(uavTextureBefore, _dxCommand);
		chunk.pTexture->GetDxResource().CreateBarrier(srvTextureBefore, _dxCommand);
	}
}
