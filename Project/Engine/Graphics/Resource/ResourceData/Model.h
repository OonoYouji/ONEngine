#pragma once

/// std
#include <memory>
#include <vector>
#include <string>

/// engine
#include "Mesh.h"

/// ===================================================
/// mesh描画クラス
/// ===================================================
class Model final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Model();
	~Model();

	/// @brief mesh の新規追加
	/// @param _mesh meshのunique_ptr
	void AddMesh(std::unique_ptr<Mesh>&& _mesh) {
		meshes_.push_back(std::move(_mesh));
	}

	Mesh* CreateMesh() {
		meshes_.emplace_back(std::make_unique<Mesh>());
		return meshes_.back().get();
	}

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<Mesh>> meshes_;
	std::string                        path_;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief mesh配列のセッタ
	/// @param _meshes unique_ptrのmesh配列
	void SetMeshes(std::vector<std::unique_ptr<Mesh>>&& _meshes);

	/// @brief パスのセッタ
	/// @param _path ファイルパス
	void SetPath(const std::string& _path) { path_ = _path; }

	/// @brief meshの配列のゲッタ
	/// @return meshの配列
	const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const { return meshes_; }

	/// @brief 読み込みに使用したファイルパスのゲッタ
	/// @return pathの文字列
	const std::string& GetPath() const { return path_; }

};
