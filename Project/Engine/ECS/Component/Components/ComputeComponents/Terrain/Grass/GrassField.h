#pragma once

/// std
#include <string>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Asset/Assets/Material/Material.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

/// interface
#include "Engine/ECS/Component/Components/Interface/IComponent.h"

namespace Editor {
class GrassArrangementPipeline;
}

namespace ONEngine {
class DxManager;
class DxDevice;
class DxCommand;
class DxSRVHeap;
class GrassField;
}

namespace ONEngine::Asset {
class AssetCollection;
}


/// ////////////////////////////////////////////////////////
/// 草のインスタンス情報 (シェーダーで利用)
/// ////////////////////////////////////////////////////////
namespace ONEngine {

/**
 * @struct GrassData
 * @brief 個々の草ブレードの発生座標、曲げ方向（接線）、スケール、およびアニメーション等で使用する乱数パラメータを保持する構造体
 */
struct GrassData {
	Vector3 position; ///< 草ブレードのワールド座標（または地形ローカル）
	Vector3 tangent;  ///< 風などによる曲がりを規定する接線ベクトル
	float scale;      ///< 草のサイズスケール
	float random01;   ///< 0.0〜1.0 のランダム値（アニメーション同期ズレ用）
};

/// ////////////////////////////////////////////////////////
/// Editor
/// ////////////////////////////////////////////////////////
namespace ComponentDebug {
    /**
     * @brief エディタ用：GrassFieldコンポーネントのデバッグ表示（Gui描画等）処理を行います。
     */
    void GrassFieldDebug(GrassField* _grassField, Asset::AssetCollection* _assetCollection);
}

/// ////////////////////////////////////////////////////////
/// json変換
/// ////////////////////////////////////////////////////////

/**
 * @brief JSONへのシリアライズ
 */
void to_json(nlohmann::json& _j, const GrassField& _p);

/**
 * @brief JSONからのデシリアライズ
 */
void from_json(const nlohmann::json& _j, GrassField& _p);

/**
 * @class GrassField
 * @brief 地形（Terrain）上に大量の草ブレード（Grass Blade）をプロシージャル配置し、GPUでのジオメトリシェーダ等を用いたインスタンシング描画を制御するコンポーネントクラス
 */
class GrassField : public IComponent {
	/// friendクラス
	friend class ::Editor::GrassArrangementPipeline;

	/// privateメンバ変数の参照のためにフレンド宣言
	friend void ComponentDebug::GrassFieldDebug(GrassField* _grassField, Asset::AssetCollection* _assetCollection);
	friend void to_json(nlohmann::json& _j, const GrassField& _p);
	friend void from_json(const nlohmann::json& _j, GrassField& _p);
public:
	/// ==================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	GrassField();

	/**
	 * @brief デストラクタ
	 */
	~GrassField();

	/**
	 * @brief 最大草ブレード数に対応するGPUバッファ（UAV構造化バッファ等）を構築・初期化します。
	 */
	void Initialize(
		uint32_t _maxBladeCount,
		DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap
	);

	/**
	 * @brief マテリアル定数バッファ等の描画に必要な定数リソースデータをセットアップ（GPUへ転送）します。
	 */
	void SetupRenderingData(Asset::AssetCollection* _assetCollection);
	/**
	 * @brief 描画コマンド用の開始インデックスバッファパラメータを設定・構築します。
	 */
	void StartIndexMapping(UINT _oneDrawInstanceCount);

	/**
	 * @brief 配置完了した有効な草ブレードインスタンス数を調べるカウンタバッファから、CPU側に本数を読み戻します。
	 */
	void AppendBufferReadCounter(DxManager* _dxm, DxCommand* _dxCommand);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- buffer ----- ///
	StructuredBuffer<GrassData> rwGrassInstanceBuffer_;
	StructuredBuffer<uint32_t> startIndexBuffer_;
	StructuredBuffer<float> timeBuffer_;
	ConstantBuffer<GPUMaterial> materialBuffer_;

	/// ----- parameters ----- ///
	uint32_t maxGrassCount_; ///< 最大草の本数
	std::string distributionTexturePath_; ///< 草の配置に使うテクスチャのパス
	bool isCreated_;
	bool isArranged_; ///< 配置済みかどうか
	uint32_t instanceCount_; ///< 実際に配置された草の本数

	Asset::Material material_;

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	/**
	 * @brief 草インスタンスデータ用の構造化バッファオブジェクトへの参照を取得します。
	 */
	StructuredBuffer<GrassData>& GetRwGrassInstanceBuffer();
	/**
	 * @brief 描画コマンド用の開始インデックス構造化バッファを取得します。
	 */
	StructuredBuffer<uint32_t>& GetStartIndexBufferRef();
	/**
	 * @brief 風アニメーション等に使う時間情報を転送する構造化バッファを取得します。
	 */
	StructuredBuffer<float>& GetTimeBuffer();
	/**
	 * @brief マテリアルパラメータ（GPUMaterial）の定数バッファを取得します。
	 */
	ConstantBuffer<GPUMaterial>& GetMaterialBufferRef();

	/**
	 * @brief 最大配置可能草ブレード数を取得します。
	 */
	uint32_t GetMaxGrassCount() const;
	/**
	 * @brief バッファ初期構築完了フラグを取得します。
	 */
	bool GetIsCreated() const;
	/**
	 * @brief 実際に配置完了した草ブレードインスタンス数を取得します。
	 */
	uint32_t GetInstanceCount() const;
};




} /// ONEngine
