#pragma once

/// std
#include <string>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Asset/Assets/Material/Material.h"
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Graphics/Buffer/Data/UVTransform.h"


namespace ONEngine {
class SpriteRenderer;
}

namespace ONEngine::Asset {
class AssetCollection;
}


namespace ONEngine {


namespace ComponentDebug {
void SpriteDebug(SpriteRenderer* _sr, Asset::AssetCollection* _assetCollection);
}

/// json serialize
void to_json(nlohmann::json& _j, const SpriteRenderer& _sr);
void from_json(const nlohmann::json& _j, SpriteRenderer& _sr);

/// ///////////////////////////////////////////////////
/// sprite描画クラス
/// ///////////////////////////////////////////////////
/**
 * @class SpriteRenderer
 * @brief ゲームオブジェクトに2Dスプライト（テクスチャ）をアタッチし、指定されたマテリアル設定（カラー、UV変形など）を用いて2D/3D空間上へ描画するスプライトレンダラーコンポーネントクラス
 */
class SpriteRenderer final : public IComponent {
	friend class SpriteUpdateSystem;
	friend class AnimationPlayer;

	friend void ComponentDebug::SpriteDebug(SpriteRenderer* _sr, Asset::AssetCollection* _assetCollection);
	friend void to_json(nlohmann::json& _j, const SpriteRenderer& _sr);
	friend void from_json(const nlohmann::json& _j, SpriteRenderer& _sr);
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	SpriteRenderer();

	/**
	 * @brief デストラクタ
	 */
	~SpriteRenderer();

	/**
	 * @brief アセットコレクションからテクスチャのバインディング情報を取得し、GPU描画に必要な定数バッファのセットアップを行います。
	 * @param _assetCollection 検索元のコレクション
	 */
	void RenderingSetup(Asset::AssetCollection* _assetCollection);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	GPUMaterial gpuMaterial_;
	Asset::Material material_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setter ----- ///

	/**
	 * @brief スプライトのカラー（RGBA）を設定します。
	 */
	void SetColor(const Vector4& _color);

	/**
	 * @brief スプライトのUV変形（オフセット、スケール等）を設定します。
	 */
	void SetUVTransform(const UVTransform& _uvTransform);

	/// ----- getter ----- ///

	/**
	 * @brief 現在のスプライトカラーを取得します。
	 */
	const Vector4& GetColor() const;

	/**
	 * @brief GPUへ転送するためのマテリアルデータを取得します。
	 */
	const GPUMaterial& GetGpuMaterial() const;

	/**
	 * @brief 現在のスプライトUV変形設定を取得します。
	 */
	const UVTransform& GetUVTransform() const;

	/**
	 * @brief スプライトが使用しているテクスチャの解像度（ピクセル幅・高さ）を取得します。
	 */
	Vector2 GetTextureSize(Asset::AssetCollection* _assetCollection) const;

	/**
	 * @brief アニメーション制御用：マテリアル参照の取得。
	 */
	Asset::Material& GetMaterialForAnimation() { return material_; }

};


/// ===================================================
/// csで使用するための関数群
/// ===================================================

namespace MonoInternalMethods {
	/// ここでコメントアウトしているのは今後実装する
	//MonoString* InternalGetTexturePath(uint64_t _nativeHandle);
	//void InternalSetTexturePath(uint64_t _nativeHandle, MonoString* _path);

	Vector4 InternalGetColor(uint64_t _nativeHandle);
	void InternalSetColor(uint64_t _nativeHandle, Vector4 _color);

	Vector2 InternalGetTextureSize(uint64_t _nativeHandle);
}

} /// ONEngine
