#pragma once

/// std
#include <vector>
#include <array>
#include <span>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Assets/Material/Material.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

#include "TerrainVertex.h"
#include "River/River.h"


namespace Editor {
class TerrainVertexEditorCompute;
}

namespace ONEngine {
class Terrain;
class EntityComponentSystem;
class DxDevice;
class DxCommand;
class DxSRVHeap;
}

namespace ONEngine::Asset {
class AssetCollection;
}


/// ComponentDebugで使用するための前方宣言
namespace ONEngine {

static const uint32_t kMaxTerrainTextureNum = 4u;

namespace ComponentDebug {
    /**
     * @brief エディタ用：Terrainコンポーネントのデバッグ表示（Gui描画等）処理を行います。
     */
    void TerrainDebug(Terrain* _terrain, EntityComponentSystem* _ecs, Asset::AssetCollection* _assetCollection);

    /**
     * @brief エディタ用：地形テクスチャスプラット編集モード用のGui描画および操作処理を行います。
     */
    bool TerrainTextureEditModeDebug(std::array<std::string, kMaxTerrainTextureNum>* _texturePaths, int32_t* _usedTextureIndex, Asset::AssetCollection* _assetCollection);
} // namespace ComponentDebug

/**
 * @brief JSONからのデシリアライズ
 */
void from_json(const nlohmann::json& _j, Terrain& _t);

/**
 * @brief JSONへのシリアライズ
 */
void to_json(nlohmann::json& _j, const Terrain& _t);


/**
 * @struct TerrainEditorInfo
 * @brief 地形エディタでのリアルタイム勾配・テクスチャペイント編集に必要なブラシ設定を保持する構造体
 */
struct TerrainEditorInfo {
	float brushRadius;        ///< ブラシの半径
	float brushStrength;      ///< ブラシの強さ
	int32_t editMode;         ///< 編集モード（勾配、テクスチャなど）
	int32_t usedTextureIndex; ///< ペイントに使用するスプラットテクスチャのインデックス
};


/**
 * @class Terrain
 * @brief 高度マップおよび複数テクスチャスプラッティングを用いた地形（Terrain）表現を管理し、GPUによる変形や描画用バッファ制御を行うコンポーネントクラス
 */
class Terrain : public IComponent {
	friend class ::Editor::TerrainVertexEditorCompute;

	friend void ComponentDebug::TerrainDebug(Terrain* _terrain, EntityComponentSystem* _ecs, Asset::AssetCollection* _assetCollection);
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

	/**
	 * @brief コンストラクタ
	 */
	Terrain();

	/**
	 * @brief デストラクタ
	 */
	~Terrain() override;

	/**
	 * @brief 地形頂点およびインデックスを保持するGPUバッファ（UAV構造化バッファ）を生成します。
	 */
	void CreateVerticesAndIndicesBuffers(DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSrvHeap);


	/**
	 * @brief 計算シェーダ等で編集したリソースを、描画（VBV/IBV経由でのレンダリング）に適したステートへ移行するバリアを生成します。
	 */
	void CreateRenderingBarriers(DxCommand* _dxCommand);

	/**
	 * @brief 描画用に移行したリソースステートを、計算・編集用の元ステート（UAV）へ復元するバリアを生成します。
	 */
	void RestoreResourceBarriers(DxCommand* _dxCommand);

	/**
	 * @brief レンダリング呼び出し用の頂点バッファビュー（D3D12_VERTEX_BUFFER_VIEW）を取得します。
	 */
	D3D12_VERTEX_BUFFER_VIEW CreateVBV();
	/**
	 * @brief レンダリング呼び出し用のインデックスバッファビュー（D3D12_INDEX_BUFFER_VIEW）を取得します。
	 */
	D3D12_INDEX_BUFFER_VIEW CreateIBV();

	/**
	 * @brief 地形の描画マテリアルパラメータ（カラー、テクスチャスロット等）をGPUバインド用のGPUMaterial構造に変換して取得します。
	 */
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

	Asset::Material material_;

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

	/**
	 * @brief 地形を構成するスプラットテクスチャアセットパスの一覧を取得します。
	 */
	const std::array<std::string, kMaxTerrainTextureNum>& GetSplatTexPaths() const;

	/// ----- buffer ----- ///

	/**
	 * @brief GPU側の地形頂点構造化バッファの読み取り専用参照を取得します。
	 */
	const StructuredBuffer<TerrainVertex>& GetRwVertices() const;

	/**
	 * @brief GPU側の地形インデックス構造化バッファの読み取り専用参照を取得します。
	 */
	const StructuredBuffer<uint32_t>& GetRwIndices() const;

	/**
	 * @brief 頂点バッファを保持する低レベルリソース（DxResource）への参照を取得します。
	 */
	DxResource& GetVerticesResource();

	/**
	 * @brief バッファ初期構築完了フラグを設定します。
	 */
	void SetIsCreated(bool _isCreated);

	/**
	 * @brief バッファ初期構築完了フラグを取得します。
	 */
	bool GetIsCreated() const;

	/**
	 * @brief 最大頂点数を取得します。
	 */
	uint32_t GetMaxVertexNum();

	/**
	 * @brief 最大インデックス数を取得します。
	 */
	uint32_t GetMaxIndexNum();

	/**
	 * @brief 地形タイリング幅（XZサイズ）を取得します。
	 */
	const Vector2& GetSize() const;

	/// ----- edit ----- ///

	/**
	 * @brief 地形エディタ情報の読み取り専用参照を取得します。
	 */
	const TerrainEditorInfo& GetEditorInfo() const;

	/// ----- river ----- ///

	/**
	 * @brief 地形上に配置された川（River）オブジェクトへのポインタを取得します。
	 */
	River* GetRiver();

	/// ----- flags ----- ///

	/**
	 * @brief プロシージャル（計算ベース）描画が有効であるかを取得します。
	 */
	bool GetIsRenderingProcedural() const;

	/**
	 * @brief プロシージャル描画の有効/無効を設定します。
	 */
	void SetIsRenderingProcedural(bool _isRenderingProcedural);

};



} /// ONEngine
