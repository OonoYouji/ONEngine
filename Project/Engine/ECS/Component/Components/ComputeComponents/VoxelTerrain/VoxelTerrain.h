#pragma once

/// std
#include <vector>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Asset/Assets/Mateiral/Material.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/VertexBuffer.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

/*
* このボクセル地形の仕様、構造について
*
* [ 基本構造 ]
* ボクセル地形は複数のチャンク(Chunk)で構成される。
* 各チャンクは3Dテクスチャ(Texture3D)で表現され、各ボクセルの情報を格納する。
* チャンクの大きさは親であるボクセル地形が決定する。
*
* [ ボクセル情報 ]
* 各ボクセルは以下の情報を持つ。
* - マテリアルID: ボクセルの材質を識別するためのID。これをもとにマテリアルテクスチャから色や質感を取得する。
*
* [ 入出力形式 ]
* チャンクのデータはDDS形式の3Dテクスチャとして保存される。
* 空のチャンクは入出力を行わないことで、パフォーマンスを最適化する。
* ファイル名をチャンクのIdに基づいて決定することで、チャンクの位置を特定できるようにする。
*
* [ 描画 ]
* AS, MS, PSを用いてボクセル地形を描画する。
* ASではチャンクの可視性を判定し、MSでジオメトリを生成、PSで最終的な色を決定する。
*
* [ 衝突判定 ]
* チャンクごとに衝突判定を行う。
* チャンクが存在しない場合は衝突判定をスキップする。
*
*/


namespace ONEngine {


struct VoxelTerrainVertex {
	Vector4 position;
	Vector4 color;
	Vector3 normal;
};


/// ///////////////////////////////////////////////////
/// ボクセル地形におけるチャンク
/// ///////////////////////////////////////////////////
struct Chunk {
	Guid texture3DId; ///< このチャンクを表現するTexture3DのId
	Texture* pTexture;
	Texture uavTexture; ///< エディタ用UAVテクスチャ

	StructuredBuffer<VoxelTerrainVertex> rwVertices;
	StructuredBuffer<uint32_t> rwVertexCounter;
	uint32_t vertexCount;
	VertexBuffer<VoxelTerrainVertex> vbv;
};

/// @brief デバッグ関数用に前方宣言をする
class VoxelTerrain;
class DxManager;
class AssetCollection;

namespace ComponentDebug {
void VoxelTerrainDebug(VoxelTerrain* _voxelTerrain, DxManager* _dxm, AssetCollection* _ac);
}

void from_json(const nlohmann::json& _j, std::vector<Chunk>& _chunk);
void to_json(nlohmann::json& _j, const std::vector<Chunk>& _chunk);


void from_json(const nlohmann::json& _j, VoxelTerrain& _voxelTerrain);
void to_json(nlohmann::json& _j, const VoxelTerrain& _voxelTerrain);

/// ///////////////////////////////////////////////////
/// GPU用のデータ構造体
/// ///////////////////////////////////////////////////
namespace GPUData {

/// @brief 地形のデータ
struct VoxelTerrainInfo {
	Vector3 terrainOrigin;
	float pad0;
	Vector3Int textureSize;
	float pad1;
	Vector3Int chunkSize;
	float pad2;
	Vector2Int chunkCountXZ; /// XZ平面でのチャンク数
	uint32_t maxChunkCount;
	float isoLevel;
};

/// @brief チャンクごとのGPU用データ
struct Chunk {
	uint32_t texture3DIndex;
};


/// @brief 編集に使う入力情報
struct InputInfo {
	Vector2 screenMousePos;
	uint32_t mouseLeftButton;
	uint32_t keyboardKShift;
};

/// @brief VoxelTerrainの編集用データ
struct EditInfo {
	float brushRadius;
};


struct MarchingCube {
	float isoValue;
	float voxelSize;
};



}


/// ///////////////////////////////////////////////////
/// ボクセルで表現された地形
/// ///////////////////////////////////////////////////
class VoxelTerrain : public IComponent {
	/// --------------- friend function --------------- ///
	friend void ComponentDebug::VoxelTerrainDebug(VoxelTerrain* _voxelTerrain, DxManager* _dxm, AssetCollection* _ac);
	friend void from_json(const nlohmann::json& _j, VoxelTerrain& _voxelTerrain);
	friend void to_json(nlohmann::json& _j, const VoxelTerrain& _voxelTerrain);

	/// --------------- friend class --------------- ///
	friend class VoxelTerrainRenderingPipeline;
	friend class VoxelTerrainVertexShaderRenderingPipeline;
	friend class VoxelTerrainVertexCreatePipeline;
	friend class VoxelTerrainTransvoxelRenderingPipeline;
public:
	/// ===========================================
	/// public : static objects
	/// ===========================================

	inline static const Vector3Int kDefaultChunkSize = Vector3Int(16, 128, 16);
	inline static const Vector2Int kChunkCount = Vector2Int(32, 32);


public:
	/// ===========================================
	/// public : methods
	/// ===========================================

	VoxelTerrain();
	~VoxelTerrain() override;


	/// @brief チャンクのGuid設定を行う
	/// @param _assetCollection AssetCollectionのポインタ
	void SettingChunksGuid(class AssetCollection* _assetCollection);

	/// @brief Bufferが生成されているかチェックする
	/// @return true: 生成済み, false: 未生成
	bool CheckCreatedBuffers() const;

	/// @brief Bufferの生成を行う
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _dxSRVHeap DxSRVHeapのポインタ
	void CreateBuffers(DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, AssetCollection* _assetCollection);

	/// @brief GraphicsPipeline用のバッファ設定を行う
	/// @param _cmdList GraphicsCommandListのポインタ
	/// @param _rootParamIndices [0]: VoxelTerrainInfo, [1]: ChunkArray
	void SetupGraphicBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 3> _rootParamIndices, class AssetCollection* _assetCollection);

	/// テクスチャのステートを変更する
	void TransitionTextureStates(class DxCommand* _dxCommand, class AssetCollection* _assetCollection, D3D12_RESOURCE_STATES _afterState);

	/// @brief 現在のチャンクの総数を取得する
	/// @return 今あるチャンクの総数
	UINT MaxChunkCount() const;

	/// @brief チャンクの大きさを取得する
	/// @return 
	const Vector2Int& GetChunkCountXZ() const;

	/// @brief チャンクの大きさを取得する
	/// @return チャンクの大きさ
	const Vector3Int& GetChunkSize() const;


	void SettingMaterial();
	void SettingTerrainInfo();


	/// --------------- エディタ用 関数 --------------- ///

	/// @brief エディタ用のバッファが生成されているかチェックする
	/// @return true: 生成済み, false: 未生成
	bool CheckBufferCreatedForEditor() const;

	/// @brief エディタ用のバッファの生成を行う
	/// @param _dxDevice DxDeviceのポインタ
	void CreateEditorBuffers(DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap);

	/// @brief エディタ用のバッファをパイプラインに設定する
	/// @param _cmdList CommandListのポインタ
	/// @param _rootParamIndices 設定するルートパラメータのインデックス配列 (0:InputInfo, 1:TerrainInfo, 2:EditInfo, 3:Chunks)
	/// @param _inputInfo InputInfo構造体
	/// @param _editInfo EditInfo構造体
	void SetupEditorBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 4> _rootParamIndices, class AssetCollection* _assetCollection, const GPUData::InputInfo& _inputInfo);

	/// @brief チャンク用のTexture3D UAVを作成する
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _dxSRVHeap DxSRVHeapのポインタ
	/// @param _assetCollection AssetCollectionのポインタ
	void CreateChunkTextureUAV(DxCommand* _dxCommand, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, class AssetCollection* _assetCollection);

	/// @brief 編集したエディタ用テクスチャをチャンク用テクスチャにコピーする
	/// @param _dxCommand DxCommandのポインタ
	/// @param _dxDevice DxDeviceのポインタ
	/// @param _assetCollection 
	void CopyEditorTextureToChunkTexture(DxCommand* _dxCommand);


	bool CanMeshShaderRendering() const { return canMeshShaderRendering_; }
	bool IsEditMode() const { return isEditMode_; }

private:
	/// ===========================================
	/// private : objects
	/// ===========================================

	/// --------------- Other Class Pointers --------------- ///
	DxSRVHeap* pDxSRVHeap_;


	std::vector<Chunk> chunks_; ///< チャンクの配列


	/// --------------- Buffer --------------- ///
	ConstantBuffer<GPUData::VoxelTerrainInfo> cBufferTerrainInfo_;
	StructuredBuffer<GPUData::Chunk> sBufferChunks_;
	StructuredBuffer<GPUData::Chunk> sBufferEditorChunks_;
	ConstantBuffer<GPUMaterial> cBufferMaterial_;

	Vector3Int chunkSize_;
	Vector3Int textureSize_;
	Vector2Int chunkCountXZ_;
	UINT maxChunkCount_;
	float isoLevel_ = 0.5f;

	Material material_;


	/// --------------- エディタ用 --------------- ///
	ConstantBuffer<GPUData::InputInfo> cBufferInputInfo_;
	ConstantBuffer<GPUData::EditInfo>  cBufferEditInfo_;
	bool isEditMode_ = false;


	ConstantBuffer<GPUData::MarchingCube> cBufferMarchingCubeInfo_;
	bool isCreatedVoxelTerrain_ = false;

	bool canMeshShaderRendering_ = true;
	bool canVertexShaderRendering_ = false;
	bool isRenderingWireframe_ = true;
	bool isRenderingTransvoxel_ = false;
	bool isRenderingCubic_ = false;

};


} /// ONEngine
