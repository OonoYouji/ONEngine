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

	SpriteRenderer();
	~SpriteRenderer();

	/// @brief 描画用データのセットアップ
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
	void SetColor(const Vector4& _color);
	void SetUVTransform(const UVTransform& _uvTransform);

	/// ----- getter ----- ///
	const Vector4& GetColor() const;

	const GPUMaterial& GetGpuMaterial() const;

	const UVTransform& GetUVTransform() const;

	Vector2 GetTextureSize(Asset::AssetCollection* _assetCollection) const;

	/// @brief アニメーション制御用マテリアルへの参照取得
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
