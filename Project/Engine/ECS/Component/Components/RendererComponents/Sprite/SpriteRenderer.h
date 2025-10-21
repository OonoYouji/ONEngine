#pragma once

/// std
#include <string>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Buffer/Data/Material.h"


class AssetCollection;
class SpriteRenderer;
namespace COMP_DEBUG {
	void SpriteDebug(SpriteRenderer* _sr, AssetCollection* _resourceCollection);
}

/// json serialize
void to_json(nlohmann::json& _j, const SpriteRenderer& _sr);
void from_json(const nlohmann::json& _j, SpriteRenderer& _sr);

/// ===================================================
/// sprite描画クラス
/// ===================================================
class SpriteRenderer final : public IComponent {
	friend class SpriteUpdateSystem;

	friend void COMP_DEBUG::SpriteDebug(SpriteRenderer* _sr, AssetCollection* _resourceCollection);
	friend void to_json(nlohmann::json& _j, const SpriteRenderer& _sr);
	friend void from_json(const nlohmann::json& _j, SpriteRenderer& _sr);
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SpriteRenderer();
	~SpriteRenderer();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	GPUMaterial gpuMaterial_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setter ----- ///
	void SetColor(const Vector4& _color);

	/// ----- getter ----- ///
	const Vector4& GetColor() const;

	const GPUMaterial& GetMaterial() const;
	GPUMaterial& GetMaterial();

};


/// ===================================================
/// csで使用するための関数群
/// ===================================================

namespace InternalSpriteMethods {
	/// ここでコメントアウトしているのは今後実装する
	//MonoString* InternalGetTexturePath(uint64_t _nativeHandle);
	//void InternalSetTexturePath(uint64_t _nativeHandle, MonoString* _path);

	Vector4 InternalGetColor(uint64_t _nativeHandle);
	void InternalSetColor(uint64_t _nativeHandle, Vector4 _color);
}
