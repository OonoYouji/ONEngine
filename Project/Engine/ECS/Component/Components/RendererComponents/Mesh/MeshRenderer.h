#pragma once

/// externals
#include <mono/jit/jit.h>
#include <nlohmann/json_fwd.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Assets/Material/Material.h"
#include "Engine/Graphics/Pipelines/Render/Mesh/MeshRenderingPipeline.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"
#include "Engine/Graphics/Buffer/Data/UVTransform.h"


namespace ONEngine {
class EntityComponentSystem;
class MeshRenderer;
}

namespace ONEngine::Asset {
class AssetCollection;
}



namespace ONEngine {

namespace ComponentDebug {
/// @brief MeshRendererのデバッグ表示
void MeshRendererDebug(MeshRenderer* _mr, Asset::AssetCollection* _assetCollection);
}

/// Json変換
void from_json(const nlohmann::json& _j, MeshRenderer& _mr);
void to_json(nlohmann::json& _j, const MeshRenderer& _mr);


/// @brief 描画の優先順位
enum class RenderQueue : uint32_t {
	Background = 0,
	Telegraph  = 1,
	Default    = 2,
};

/// ///////////////////////////////////////////////////
/// mesh描画クラス
/// ///////////////////////////////////////////////////
class MeshRenderer : public IRenderComponent {
	friend class AnimationPlayer;
	/// friend methods
	friend void ComponentDebug::MeshRendererDebug(MeshRenderer* _mr, Asset::AssetCollection* _assetCollection);
	friend void from_json(const nlohmann::json& _j, MeshRenderer& _mr);
	friend void to_json(nlohmann::json& _j, const MeshRenderer& _mr);

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderer();
	~MeshRenderer();

	/// @brief 描画のために必要なデータを設定する
	void SetupRenderData(Asset::AssetCollection* _assetCollection);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string meshPath_;

	GPUMaterial gpuMaterial_;
	Asset::Material material_;

	RenderQueue renderQueue_ = RenderQueue::Default;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief 描画するレイヤーの設定
	void SetRenderQueue(RenderQueue _queue);

	/// @brief 描画レイヤーの取得
	RenderQueue GetRenderQueue() const;

	/// @brief 描画するmeshの file pathを設定
	/// @param _path .slnからの相対パス
	void SetMeshPath(const std::string& _path);

	/// @brief 描画する色の設定
	/// @param _color RGBA 0.0f ~ 1.0f
	void SetColor(const Vector4& _color);

	/// @brief ポストエフェクトのフラグを設定
	/// @param _flags ポストエフェクトのフラグ
	void SetPostEffectFlags(uint32_t _flags);

	/// @brief UV変形のセット
	void SetUVTransform(const UVTransform& _uvTransform);

	/// @brief 描画するmeshの file pathを取得
	/// @return .slnからの相対パス
	const std::string& GetMeshPath() const;

	/// @brief 色の取得
	/// @return RGBA 0.0f ~ 1.0f
	const Vector4& GetColor() const;

	/// @brief GPUで使用するMaterialデータの取得
	const GPUMaterial& GetGpuMaterial() const;

	/// @brief ポストエフェクトのフラグを取得
	/// @return ポストエフェクトのフラグ
	uint32_t GetPostEffectFlags() const;

	/// @brief UV変形の取得
	const UVTransform& GetUVTransform() const;

	/// @brief テクスチャのGuidを返す
	const Guid& GetTextureGuid() const;

	/// @brief アニメーション制御用マテリアルへの参照取得
	Asset::Material& GetMaterialForAnimation() { return material_; }

};



/// ===================================================
/// csで使用するための関数群
/// ===================================================

MonoString* InternalGetMeshName(uint64_t _nativeHandle);
void InternalSetMeshName(uint64_t _nativeHandle, MonoString* _meshName);
Vector4 InternalGetMeshColor(uint64_t _nativeHandle);
void InternalSetMeshColor(uint64_t _nativeHandle, Vector4 _color);
uint32_t InternalGetPostEffectFlags(uint64_t _nativeHandle);
void InternalSetPostEffectFlags(uint64_t _nativeHandle, uint32_t _flags);
uint32_t InternalGetRenderQueue(uint64_t _nativeHandle);
void InternalSetRenderQueue(uint64_t _nativeHandle, uint32_t _queue);

} /// ONEngine
