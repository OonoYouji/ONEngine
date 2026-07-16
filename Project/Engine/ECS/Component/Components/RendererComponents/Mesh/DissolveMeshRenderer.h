#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Asset/Assets/Material/Material.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

namespace ONEngine {
class DissolveMeshRenderer;
}

namespace ONEngine::Asset {
class AssetCollection;
}


namespace ONEngine {

/// ///////////////////////////////////////////////////
/// ディゾルブの比較方法
/// ///////////////////////////////////////////////////
enum class DissolveCompare {
	LessEqual,
	GreaterEqual
};

/// ///////////////////////////////////////////////////
/// メッシュをディゾルブ表現で表示するためのコンポーネント
/// ///////////////////////////////////////////////////
/**
 * @class DissolveMeshRenderer
 * @brief メッシュに対してディゾルブ（徐々に消えていく、あるいは現れる）表現を適用するための特殊描画レンダラーコンポーネントクラス
 */
class DissolveMeshRenderer : public IRenderComponent {
	friend class AnimationPlayer;
	friend void ShowGUI(DissolveMeshRenderer* _dmr, Asset::AssetCollection* _ac);
	friend void from_json(const nlohmann::json& _j, DissolveMeshRenderer& _dmr);
	friend void to_json(nlohmann::json& _j, const DissolveMeshRenderer& _dmr);
public:
	/// ===========================================
	/// public : methods
	/// ===========================================

	/**
	 * @brief コンストラクタ
	 */
	DissolveMeshRenderer();
	/**
	 * @brief デストラクタ
	 */
	~DissolveMeshRenderer();

private:
	/// ===========================================
	/// private : objects
	/// ===========================================

	Guid meshGuid_;
	Asset::Material material_;
	Guid dissolveTexture_;

	float dissolveThreshold_ = 1.0f;
	
	DissolveCompare dissolveCompare_ = DissolveCompare::LessEqual;

	float edgeWidth_ = 0.05f;
	Vector4 edgeColor_ = { 1.0f, 0.5f, 0.0f, 1.0f }; // Orange glow default

	friend class AnimationPlayer;

public:
	/// ===========================================
	/// public : accessors
	/// ===========================================

	/**
	 * @brief 描画するメッシュアセット的Guidを取得します。
	 */
	const Guid& GetMeshGuid() const;
	/**
	 * @brief 境界しきい値算出用のディゾルブノイズテクスチャのGuidを取得します。
	 */
	const Guid& GetDissolveTextureGuid() const;

	/**
	 * @brief アセットコレクションからディゾルブテクスチャのバインドインデックス（SRV）を取得します。
	 */
	uint32_t GetDissolveTextureId(Asset::AssetCollection* _ac) const;
	/**
	 * @brief 現在のディゾルブの進行度しきい値（0.0f = 完全消失, 1.0f = 完全表示等）を取得します。
	 */
	float GetDissolveThreshold() const;

	/**
	 * @brief GPUへ転送するためのマテリアル定数データをパックして取得します。
	 */
	GPUMaterial GetGPUMaterial(Asset::AssetCollection* _ac) const;

	/**
	 * @brief ディゾルブ比較処理のタイプ（LessEqual/GreaterEqual）を取得します。
	 */
	uint32_t GetDissolveCompare() const;

	/**
	 * @brief ディゾルブ境界（エッジ）の光る幅を取得します。
	 */
	float GetEdgeWidth() const { return edgeWidth_; }
	/**
	 * @brief ディゾルブ境界（エッジ）の発光カラーを取得します。
	 */
	const Vector4& GetEdgeColor() const { return edgeColor_; }


	/**
	 * @brief ディゾルブのしきい値を設定します。
	 */
	void SetThreshold(float threshold) {
		dissolveThreshold_ = threshold;
	}

	/**
	 * @brief UV変形（マテリアル）を設定します。
	 */
	void SetUVTransform(const UVTransform& _uvTransform) {
		material_.uvTransform = _uvTransform;
	}

	/**
	 * @brief マテリアルのUV変形設定を取得します。
	 */
	const UVTransform& GetUVTransform() const {
		return material_.uvTransform;
	}

	/**
	 * @brief アニメーション制御用：マテリアル参照の取得。
	 */
	Asset::Material& GetMaterialForAnimation() { return material_; }
	
	/**
	 * @brief アニメーション制御用：しきい値変数への参照を取得。
	 */
	float& GetThresholdForAnimation() { return dissolveThreshold_; }

	/**
	 * @brief アニメーション制御用：エッジ幅変数への参照を取得。
	 */
	float& GetEdgeWidthForAnimation() { return edgeWidth_; }

	/**
	 * @brief アニメーション制御用：エッジカラー変数への参照を取得。
	 */
	Vector4& GetEdgeColorForAnimation() { return edgeColor_; }

};

} /// namespace ONEngine