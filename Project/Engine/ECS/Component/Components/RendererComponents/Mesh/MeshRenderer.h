#pragma once

/// externals
#include <mono/jit/jit.h>
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Pipelines/Render/Mesh/MeshRenderingPipeline.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"


/// 前方宣言
class MeshRenderer;
class AssetCollection;

namespace COMP_DEBUG {
	/// @brief MeshRendererのデバッグ表示
	void MeshRendererDebug(MeshRenderer* _mr, AssetCollection* _grc);
}

void from_json(const nlohmann::json& _j, MeshRenderer& _mr);
void to_json(nlohmann::json& _j, const MeshRenderer& _mr);



/// ===================================================
/// mesh描画クラス
/// ===================================================
class MeshRenderer : public IRenderComponent {
	/// friend methods
	friend void COMP_DEBUG::MeshRendererDebug(MeshRenderer* _mr, AssetCollection* _grc);
	friend void from_json(const nlohmann::json& _j, MeshRenderer& _mr);
	friend void to_json(nlohmann::json& _j, const MeshRenderer& _mr);

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderer();
	~MeshRenderer();

	/// @brief 描画のために必要なデータを設定する
	void SetupRenderData(class AssetCollection* _grc);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string meshPath_;
	std::string texturePath_;

	GPUMaterial gpuMaterial_;
	Material    cpuMaterial_;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief 描画するmeshの file pathを設定
	/// @param _path .slnからの相対パス
	void SetMeshPath(const std::string& _path);

	/// @brief 描画に使用するtextureの file pathを設定
	/// @param _path .slnからの相対パス
	void SetTexturePath(const std::string& _path);

	/// @brief 描画する色の設定
	/// @param _color RGBA 0.0f ~ 1.0f
	void SetColor(const Vector4& _color);

	/// @brief ポストエフェクトのフラグを設定
	/// @param _flags ポストエフェクトのフラグ
	void SetPostEffectFlags(uint32_t _flags);

	void SetMaterialEntityId();

	/// @brief 描画するmeshの file pathを取得
	/// @return .slnからの相対パス
	const std::string& GetMeshPath() const;

	/// @brief 描画に使用するtextureの file pathを取得
	/// @return .slnからの相対パス
	const std::string& GetTexturePath() const;

	/// @brief 色の取得
	/// @return RGBA 0.0f ~ 1.0f
	const Vector4& GetColor() const;

	const GPUMaterial& GetMaterial() const;
	const Material&    GetCPUMaterial() const;

	/// @brief ポストエフェクトのフラグを取得
	/// @return ポストエフェクトのフラグ
	uint32_t GetPostEffectFlags() const;

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