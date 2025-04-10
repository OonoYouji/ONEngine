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
};


