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
/**
 * @class MeshRenderer
 * @brief ゲームオブジェクトに静的メッシュ（Mesh）をアタッチし、指定されたマテリアル設定（カラー、テクスチャ、ポストエフェクト等）を用いて3D描画システムへ登録するレンダラーコンポーネントクラス
 */
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

	/**
	 * @brief コンストラクタ
	 */
	MeshRenderer();

	/**
	 * @brief デストラクタ
	 */
	~MeshRenderer();

	/**
	 * @brief アセットコレクションから、指定されたパスのメッシュ・マテリアル・テクスチャデータを取得し、描画に必要なGPU用定数バッファのセットアップを行います。
	 * @param _assetCollection アセット検索元
	 */
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

	/**
	 * @brief 描画プライオリティ（RenderQueue）を設定します（Background, Telegraph, Default等）。
	 */
	void SetRenderQueue(RenderQueue _queue);

	/**
	 * @brief 現在の描画プライオリティを取得します。
	 */
	RenderQueue GetRenderQueue() const;

	/**
	 * @brief 描画に使用するメッシュアセットのファイルパス（.objや.fbx等）を設定します。
	 * @param _path プロジェクトルートからのアセット相対パス
	 */
	void SetMeshPath(const std::string& _path);

	/**
	 * @brief 描画カラー（RGBA）を設定します。
	 * @param _color カラー値（各成分 0.0f ~ 1.0f）
	 */
	void SetColor(const Vector4& _color);

	/**
	 * @brief ポストエフェクト描画制御フラグを設定します（アウトライン、Dissolve等）。
	 * @param _flags フラグ値
	 */
	void SetPostEffectFlags(uint32_t _flags);

	/**
	 * @brief UVトランスフォーム（テクスチャのタイリングやオフセットなど）を設定します。
	 */
	void SetUVTransform(const UVTransform& _uvTransform);

	/**
	 * @brief 設定されているメッシュアセットのファイルパスを取得します。
	 */
	const std::string& GetMeshPath() const;

	/**
	 * @brief 設定されているカラー値を取得します。
	 */
	const Vector4& GetColor() const;

	/**
	 * @brief 定数バッファ送信用にパックされたGPUマテリアルデータ（GPUMaterial）を取得します。
	 */
	const GPUMaterial& GetGpuMaterial() const;

	/**
	 * @brief ポストエフェクト描画制御フラグを取得します。
	 */
	uint32_t GetPostEffectFlags() const;

	/**
	 * @brief UVトランスフォームパラメータを取得します。
	 */
	const UVTransform& GetUVTransform() const;

	/**
	 * @brief アタッチされているカラーマップテクスチャのGuidを取得します。
	 */
	const Guid& GetTextureGuid() const;

	/**
	 * @brief アニメーション制御等でマテリアル状態を動的に変更するための非constマテリアル参照を取得します。
	 */
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
