#pragma once


/// std
#include <vector>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Assets/Mesh/Model.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

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

	/// @brief meshの再作成
	/// @param _pDxDevice DxDeviceへのポインタ
	void MeshRecreate(class DxDevice* _pDxDevice);

	void VertexMemcpy();

private:
	/// ===================================================
	/// private : objects
	/// ====================================================

	Mesh mesh_;
	std::string texturePath_ = "Packages/Textures/uvChecker.png";
	bool isVisible_ = true; ///< 描画するかどうか
	bool isBufferRecreate_ = false; ///< バッファを再作成するかどうか

	GPUMaterial gpuMaterial_;

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

	/// @brief textureのpathのセッタ
	/// @param _path Textureのpath
	void SetTexturePath(const std::string& _path);

	/// @brief 色の設定
	/// @param _color RGBAの色を設定する
	void SetColor(const Vector4& _color);

	/// @brief 描画するかどうかのセッタ
	/// @param _isVisible 描画フラグ
	void SetIsVisible(bool _isVisible);

	/// @brief bufferを再作成するかどうかのセッタ
	/// @param _isBufferRecreate bufferを再作成するフラグ
	void SetIsBufferRecreate(bool _isBufferRecreate);



	/// @brief textureのpathのゲッタ
	/// @return textureのpath
	const std::string& GetTexturePath() const;

	/// @brief 色のゲッタ
	/// @return RGBAの色
	const Vector4& GetColor() const;

	/// @brief meshのゲッタ
	/// @return Meshのポインタ
	const Mesh* GetMesh() const;

	/// @brief 描画するかどうかのゲッタ
	/// @return 描画フラグ
	bool GetIsVisible() const;

	/// @brief bufferを再作成するかどうかのゲッタ
	/// @return bufferを再作成するフラグ
	bool GetIsBufferRecreate() const;

	const GPUMaterial& GetMaterial();

};


