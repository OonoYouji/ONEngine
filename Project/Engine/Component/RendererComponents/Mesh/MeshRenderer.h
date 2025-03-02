#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Pipelines/Mesh/MeshRenderingPipeline.h"

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

	Vector4 color_ = Vector4::kWhite;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief 描画するmeshの file pathを設定
	/// @param _path .slnからの相対パス
	void SetMeshPath(const std::string& _path) { meshPath_ = _path; }

	/// @brief 描画に使用するtextureの file pathを設定
	/// @param _path .slnからの相対パス
	void SetTexturePath(const std::string& _path) { texturePath_ = _path; }

	/// @brief 描画する色の設定
	/// @param _color RGBA 0.0f ~ 1.0f
	void SetColor(const Vector4& _color) { color_ = _color; }


	/// @brief 描画するmeshの file pathを取得
	/// @return .slnからの相対パス
	const std::string& GetMeshPath() const { return meshPath_; }

	/// @brief 描画に使用するtextureの file pathを取得
	/// @return .slnからの相対パス
	const std::string& GetTexturePath() const { return texturePath_; }

	/// @brief 色の取得
	/// @return RGBA 0.0f ~ 1.0f
	const Vector4& GetColor() const { return color_; }

};

