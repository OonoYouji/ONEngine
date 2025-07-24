#pragma once

/// externals
#include <mono/jit/jit.h>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Pipelines/Render/Mesh/MeshRenderingPipeline.h"
#include "Engine/Graphics/Buffer/Data/Material.h"

/// ===================================================
/// mesh描画クラス
/// ===================================================
class MeshRenderer : public IRenderComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderer();
	~MeshRenderer();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string meshPath_;
	std::string texturePath_;

	Material material_;

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

	/// @brief 描画するmeshの file pathを取得
	/// @return .slnからの相対パス
	const std::string& GetMeshPath() const;

	/// @brief 描画に使用するtextureの file pathを取得
	/// @return .slnからの相対パス
	const std::string& GetTexturePath() const;

	/// @brief 色の取得
	/// @return RGBA 0.0f ~ 1.0f
	const Vector4& GetColor() const;

	const Material& GetMaterial() const;

	/// @brief ポストエフェクトのフラグを取得
	/// @return ポストエフェクトのフラグ
	uint32_t GetPostEffectFlags() const;

};


namespace COMP_DEBUG {

	/// @brief MeshRendererのデバッグ表示
	void MeshRendererDebug(MeshRenderer* _mr);

}


/// ===================================================
/// csで使用するための関数群
/// ===================================================

MonoString* InternalGetMeshName(uint64_t _nativeHandle);
void InternalSetMeshName(uint64_t _nativeHandle, MonoString* _meshName);
Vector4 InternalGetMeshColor(uint64_t _nativeHandle);
void InternalSetMeshColor(uint64_t _nativeHandle, Vector4 _color);
uint32_t InternalGetPostEffectFlags(uint64_t _nativeHandle);
void InternalSetPostEffectFlags(uint64_t _nativeHandle, uint32_t _flags);