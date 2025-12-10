#pragma once

/// std
#include <vector>
#include <array>
#include <span>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Assets/Mateiral/Material.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

#include "TerrainVertex.h"
#include "River/River.h"


/// COMP_DEBUGで使用するための前方宣言
namespace ONEngine {

class Terrain;
class EntityComponentSystem;
class AssetCollection;


static const uint32_t kMaxTerrainTextureNum = 4u;



namespace COMP_DEBUG {
void TerrainDebug(Terrain* _terrain, EntityComponentSystem* _ecs, AssetCollection* _assetCollection);

/// テクスチャモードの編集
bool TerrainTextureEditModeDebug(std::array<std::string, kMaxTerrainTextureNum>* _texturePaths, int32_t* _usedTextureIndex, AssetCollection* _assetCollection);

} // namespace COMP_DEBUG

void from_json(const nlohmann::json& _j, Terrain& _t);
void to_json(nlohmann::json& _j, const Terrain& _t);

/// ///////////////////////////////////////////////////
/// 地形のエディター情報
/// ///////////////////////////////////////////////////
struct TerrainEditorInfo {
	float brushRadius;        ///< ブラシの半径
	float brushStrength;      ///< ブラシの強さ
	int32_t editMode;         ///< 編集モード
	int32_t usedTextureIndex; ///< 使用しているテクスチャのインデックス
};


/// ///////////////////////////////////////////////////
/// 地形のコンポーネント
/// ///////////////////////////////////////////////////
class Terrain : public IComponent {
	friend class TerrainVertexEditorCompute;

	friend void COMP_DEBUG::TerrainDebug(Terrain* _terrain, EntityComponentSystem* _ecs, AssetCollection* _assetCollection);
	friend void from_json(const nlohmann::json& _j, Terrain& _t);
	friend void to_json(nlohmann::json& _j, const Terrain& _t);
public:
	/// =========================================
	/// public : sub class
	/// =========================================

	enum SPLAT_TEX {
		GRASS,
		DIRT,
		ROCK,
		SNOW,
		SPLAT_TEX_COUNT
	};


	enum class EditMode : int32_t {
		None,    /// 操作なし
		Vertex,	 /// 勾配の操作
		Texture, /// テクスチャの操作
		Count
	};


public:
	/// =========================================
	/// public : methods
	/// =========================================

	Terrain();
	~Terrain() override;

	/// @brief VerticesとIndicesのUAVBufferを作成する
	void CreateVerticesAndIndicesBuffers(class DxDevice* _dxDevice, class DxCommand* _dxCommand, class DxSRVHeap* _dxSrvHeap);


	/// @brief VBVとIBVのバリアを生成する(描画用に)
	/// @param _dxCommand DxCommandへのポインタ
	void CreateRenderingBarriers(class DxCommand* _dxCommand);

	/// @brief VBVとIBVのバリアを復元する(計算用に)
	/// @param _dxCommand DxCommandへのポインタ
	void RestoreResourceBarriers(class DxCommand* _dxCommand);

	/// @brief 描画用にVBVを生成する
	D3D12_VERTEX_BUFFER_VIEW CreateVBV();
	/// @brief 描画用にIBVを生成する
	D3D12_INDEX_BUFFER_VIEW CreateIBV();

	/// @brief 自身のMaterialデータをGPUマテリアルデータに変換して返す
	/// @return GPUMaterialデータ
	GPUMaterial GetMaterialData();

private:
	/// =========================================
	/// private : objects
	/// =========================================

	/// ----- buffer ----- ///
	StructuredBuffer<TerrainVertex> rwVertices_;
	StructuredBuffer<uint32_t> rwIndices_;
	bool isCreated_;

	/// ----- edit ----- ///
	TerrainEditorInfo editorInfo_;

	/// ----- terrain ----- ///
	Vector2 terrainSize_ = Vector2(1000.0f, 1000.0f); ///< 地形のサイズ
	uint32_t maxVertexNum_;
	uint32_t maxIndexNum_;

	Material material_;

	/// ----- river ----- ///
	River river_;

	/// ----- splatting ----- ///
	std::array<std::string, kMaxTerrainTextureNum> splattingTexPaths_;

	/// ----- flags ----- ///
	bool isRenderingProcedural_;


public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	const std::array<std::string, kMaxTerrainTextureNum>& GetSplatTexPaths() const;

	/// ----- buffer ----- ///
	const StructuredBuffer<TerrainVertex>& GetRwVertices() const;
	const StructuredBuffer<uint32_t>& GetRwIndices() const;
	DxResource& GetVerticesResource();

	void SetIsCreated(bool _isCreated);
	bool GetIsCreated() const;

	uint32_t GetMaxVertexNum();
	uint32_t GetMaxIndexNum();

	const Vector2& GetSize() const;

	/// ----- edit ----- ///
	const TerrainEditorInfo& GetEditorInfo() const;

	/// ----- river ----- ///
	River* GetRiver();

	/// ----- flags ----- ///
	bool GetIsRenderingProcedural() const;
	void SetIsRenderingProcedural(bool _isRenderingProcedural);

};



} /// ONEngine
