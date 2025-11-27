#pragma once

/// std
#include <vector>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/Math/Vector3Int.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

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



/// ///////////////////////////////////////////////////
/// ボクセル地形におけるチャンク
/// ///////////////////////////////////////////////////
struct Chunk {
	/*
	*  [ 必要なデータ ]
	* このチャンクを表現するTexture3D (.dds形式)
	* チャンクのId (座標から計算可能)
	*/

	Guid texture3DId; ///< このチャンクを表現するTexture3DのId
};

/// @brief デバッグ関数用に前方宣言をする
class VoxelTerrain;

namespace COMP_DEBUG {
	void VoxelTerrainDebug(VoxelTerrain* _voxelTerrain);
}

void from_json(const nlohmann::json& _j, VoxelTerrain& _voxelTerrain);
void to_json(nlohmann::json& _j, const VoxelTerrain& _voxelTerrain);


/// ///////////////////////////////////////////////////
/// GPU用のデータ構造体
/// ///////////////////////////////////////////////////
namespace GPUData {

	/// @brief 地形のデータ
	struct VoxelTerrainInfo {
		Vector3Int chunkSize;
		float pad0;
		Vector2Int chunkCountXZ; /// XZ平面でのチャンク数
		uint32_t maxChunkCount;
		//uint32_t startTextureIndex; /// テクスチャ配列の開始インデックス
	};

	/// @brief チャンクごとのGPU用データ
	struct Chunk {
		uint32_t texture3DIndex;
	};
}


/// ///////////////////////////////////////////////////
/// ボクセルで表現された地形
/// ///////////////////////////////////////////////////
class VoxelTerrain : public IComponent {
	/// --------------- friend function --------------- ///
	friend void COMP_DEBUG::VoxelTerrainDebug(VoxelTerrain* _voxelTerrain);
	friend void from_json(const nlohmann::json& _j, VoxelTerrain& _voxelTerrain);
	friend void to_json(nlohmann::json& _j, const VoxelTerrain& _voxelTerrain);

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
	void CreateBuffers(DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap);

	/// @brief GraphicsPipeline用のバッファ設定を行う
	/// @param _cmdList GraphicsCommandListのポインタ
	/// @param _rootParamIndices [0]: VoxelTerrainInfo, [1]: ChunkArray
	void SetupGraphicBuffers(ID3D12GraphicsCommandList* _cmdList, const std::array<UINT, 2> _rootParamIndices, class AssetCollection* _assetCollection);

	/// @brief 現在のチャンクの総数を取得する
	/// @return 今あるチャンクの総数
	UINT MaxChunkCount() const;

	/// @brief チャンクの大きさを取得する
	/// @return 
	const Vector2Int& GetChunkCountXZ() const;

	/// @brief チャンクの大きさを取得する
	/// @return チャンクの大きさ
	const Vector3Int& GetChunkSize() const;

private:
	/// ===========================================
	/// private : objects
	/// ===========================================

	/*
	*  [ 必要なデータ ]
	* Chunkの配列
	* １つあたりのチャンクの大きさ (x*y*z)
	*/

	std::vector<Chunk> chunks_; ///< チャンクの配列


	/// --------------- Buffer --------------- ///
	ConstantBuffer<GPUData::VoxelTerrainInfo> cBufferTerrainInfo_;
	StructuredBuffer<GPUData::Chunk> sBufferChunks_;

	Vector3Int chunkSize_;
	Vector2Int chunkCountXZ_;
	UINT maxChunkCount_;

};

