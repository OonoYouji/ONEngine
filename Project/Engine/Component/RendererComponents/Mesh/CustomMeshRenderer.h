#pragma once


/// std
#include <vector>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Resource/ResourceData/Model.h"

/// /////////////////////////////////////////////////
/// CustomMeshRenderer
/// ///////////////////////////////////////////////
class CustomMeshRenderer final : public IRenderComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	CustomMeshRenderer();
	~CustomMeshRenderer();


private:
	/// ===================================================
	/// private : objects
	/// ====================================================

	Model model_;
	Mesh* frontMesh_ = nullptr;
	std::string texturePath_ = "Assets/Textures/white.png";
	Vector4 color_ = Vector4::kWhite; ///< RGBA 0.0f ~ 1.0f

public:
	/// ====================================================
	/// public : accessor
	/// ====================================================

	/// @brief verticesのセッタ
	/// @param _vertices meshの頂点データ
	void SetVertices(const std::vector<Mesh::VertexData>& _vertices);

	/// @brief indicesのセッタ
	/// @param _indices 頂点インデックスデータ
	void SetIndices(const std::vector<uint32_t>& _indices);

	/// @brief meshのpathのセッタ
	/// @param _path 
	void SetMeshPath(const std::string& _path) { model_.SetPath(_path); }

	/// @brief textureのpathのセッタ
	/// @param _path Textureのpath
	void SetTexturePath(const std::string& _path) { texturePath_ = _path; }

	/// @brief 色の設定
	/// @param _color RGBAの色を設定する
	void SetColor(const Vector4& _color) { color_ = _color; }


	/// @brief meshのpathのゲッタ
	/// @return Meshのpath
	const std::string& GetMeshPath() const { return model_.GetPath(); }

	/// @brief textureのpathのゲッタ
	/// @return textureのpath
	const std::string& GetTexturePath() const { return texturePath_; }

	/// @brief 色のゲッタ
	/// @return RGBAの色
	const Vector4& GetColor() const { return color_; }

	/// @brief modelへのゲッタ
	/// @return このクラスが保持するモデルのポインタ
	const Model* GetModel() const { return &model_; }

};


