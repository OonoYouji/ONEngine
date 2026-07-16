#pragma once

/// std
#include <vector>
#include <cstdint>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

/// ///////////////////////////////////////////////////
/// 川のコントロールポイント
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @struct RiverControlPoint
 * @brief Spline曲線を構成する川の制御点（位置座標と川幅）
 */
struct RiverControlPoint {
	Vector3 position; ///< 制御点の座標
	float width;      ///< この点における川の幅
};

/**
 * @struct RiverVertex
 * @brief GPU側の計算シェーダ等で出力される川のメッシュ用頂点構造体
 */
struct RiverVertex {
	Vector4 position; ///< 座標
	Vector2 uv;       ///< UV座標
	Vector3 normal;   ///< 法線
};


/**
 * @brief Catmull-Rom スプライン曲線上の点を計算補間します。
 */
RiverControlPoint CatmullRom(
	const RiverControlPoint& _p0,
	const RiverControlPoint& _p1,
	const RiverControlPoint& _p2,
	const RiverControlPoint& _p3,
	float _t
);

/**
 * @brief 制御点配列から指定サンプリング密度でスプラインに沿った補間点配列を生成します。
 */
std::vector<RiverControlPoint> SampleRiverSpline(
	const std::vector<RiverControlPoint>& _points,
	int _samplePerSegment
);


/**
 * @class River
 * @brief 地形（Terrain）上にスプライン曲線を用いた川（River）を定義し、GPU（計算シェーダ）を介して動的な水流用メッシュを自動生成・管理するクラス
 */
class River {
public:

	/**
	 * @struct Param
	 * @brief 計算シェーダ等に川のメッシュ構成設定（セグメント数、頂点数等）を転送するための定数構造体
	 */
	struct Param {
		uint32_t totalSegments;
		uint32_t totalVertices;
		uint32_t totalSamples;
		uint32_t samplePerSegment;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	River();

	/**
	 * @brief デストラクタ
	 */
	~River();

	/**
	 * @brief エディタ用：川のパラメータや制御点の追加・削除など編集用Guiの描画処理を行います。
	 */
	void Edit(class EntityComponentSystem* _ecs);

	/**
	 * @brief 川の制御点設定を外部JSONファイルへ保存します。
	 */
	void SaveToJson(const std::string& _name);

	/**
	 * @brief 外部JSONファイルから川の制御点設定を読み込みます。
	 */
	void LoadFromJson(const std::string& _name);


	/**
	 * @brief シーン上にスプラインのパス線分や制御点用ギズモ（エディタ用）を描画します。
	 */
	void DrawSplineCurve();

	/**
	 * @brief GPU側で川メッシュを格納・変形するためのUAV/CBVバッファを生成します。
	 */
	void CreateBuffers(class DxDevice* _dxDevice, class DxSRVHeap* _dxSRVHeap, class DxCommand* _dxCommand);

	/**
	 * @brief 制御点バッファ等へCPU側のデータをアップロード・転送します。
	 */
	void SetBufferData();


	/**
	 * @brief 川用の水面マテリアルパラメータ（描画時のテクスチャインデックスやOwner Entity情報）を設定します。
	 */
	void SetMaterialData(int32_t _entityId, int32_t _texIndex);


	/**
	 * @brief 計算シェーダ等で変形したバッファを、描画（VBV/IBV経由でのレンダリング）に適したステートへ移行するバリアを生成します。
	 */
	void CreateRenderingBarriers(class DxCommand* _dxCommand);

	/**
	 * @brief 描画用に移行したリソースステートを、計算用の元ステート（UAV）へ復元するバリアを生成します。
	 */
	void RestoreResourceBarriers(class DxCommand* _dxCommand);


	/**
	 * @brief レンダリング呼び出し用の頂点バッファビュー（D3D12_VERTEX_BUFFER_VIEW）を取得します。
	 */
	D3D12_VERTEX_BUFFER_VIEW CreateVBV();

	/**
	 * @brief レンダリング呼び出し用のインデックスバッファビュー（D3D12_INDEX_BUFFER_VIEW）を取得します。
	 */
	D3D12_INDEX_BUFFER_VIEW CreateIBV();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// splineのコントロールポイント
	std::vector<RiverControlPoint> controlPoints_;
	std::vector<RiverControlPoint> createdPoints_;
	int samplePerSegment_;

	/// buffer
	ConstantBuffer<Param> paramBuf_;
	ConstantBuffer<GPUMaterial> materialBuffer_;
	StructuredBuffer<RiverControlPoint> controlPointBuf_;
	StructuredBuffer<RiverVertex> rwVertices_;
	StructuredBuffer<uint32_t> rwIndices_;
	bool isCreatedBuffers_;
	UINT totalVertices_;
	UINT totalIndices_;

	/// edit
	bool isGenerateMeshRequest_;

public:
	/// ==================================================
	/// public : accessors
	/// ==================================================

	/**
	 * @brief セグメント辺あたりの補間サンプル数を取得します。
	 */
	int GetSamplePerSegment() const;

	/**
	 * @brief 現在登録されている制御点（コントロールポイント）の数を取得します。
	 */
	int GetNumControlPoint() const;

	/**
	 * @brief 川メッシュの動的再生成要求フラグを取得します。
	 */
	bool GetIsGenerateMeshRequest() const;

	/**
	 * @brief 川メッシュの動的再生成要求フラグを設定します。
	 */
	void SetIsGenerateMeshRequest(bool _request);

	/**
	 * @brief 定数バッファ（Param）を取得します。
	 */
	const ConstantBuffer<Param>& GetParamBuffer() const;

	/**
	 * @brief マテリアル定数バッファ（GPUMaterial）を取得します。
	 */
	const ConstantBuffer<GPUMaterial>& GetMaterialBuffer() const;

	/**
	 * @brief 生成された川頂点を保持する構造化バッファの読み取り専用参照を取得します。
	 */
	const StructuredBuffer<RiverVertex>& GetRwVertices() const;

	/**
	 * @brief 生成された川インデックスを保持する構造化バッファの読み取り専用参照を取得します。
	 */
	const StructuredBuffer<uint32_t>& GetRwIndices() const;

	/**
	 * @brief 制御点配列のGPU構造化バッファを取得します。
	 */
	const StructuredBuffer<RiverControlPoint>& GetControlPointBuffer() const;

	/**
	 * @brief 各バッファが生成済みであるかを取得します。
	 */
	bool GetIsCreatedBuffers() const;

	/**
	 * @brief 川インデックスバッファの総要素数を取得します。
	 */
	UINT GetTotalIndices() const;

	/**
	 * @brief 川頂点バッファの総要素数を取得します。
	 */
	UINT GetTotalVertices() const;
};

} /// ONEngine
