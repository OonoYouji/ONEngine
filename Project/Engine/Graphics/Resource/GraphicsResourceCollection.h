#pragma once

/// std
#include <memory>
#include <unordered_map>

/// engine
#include "Loader/GraphicsResourceLoader.h"
#include "ResourceData/Mesh.h"


/// ===================================================
/// グラフィクスリソースのコレクション
/// ===================================================
class GraphicsResourceCollection final {
public:

	/// ===================================================
	/// public : sub class, enum
	/// ===================================================

	/// @brief resourceの種類
	enum class Type {
		none,
		texture,
		model,
	};


public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	GraphicsResourceCollection();
	~GraphicsResourceCollection();

	/// @brief 初期化関数
	/// @param _dxManager DxManagerのポインタ
	void Initialize(class DxManager* _dxManager);

	/// @brief リソースの一括読み込み
	/// @param _filePaths リソースへのパス配列
	void LoadResources(const std::vector<std::string>& _filePaths);

	/// @brief リソースの一括解放
	/// @param _filePaths 解放するリソースへのパス配列
	void UnloadResources(const std::vector<std::string>& _filePaths);

	/// @brief リソースの読み込み
	/// @param _filePath ファイルパス
	/// @param _type 読み込むリソースの種類
	void Load(const std::string& _filePath, Type _type);


	/// @brief 新しいmeshの追加
	/// @param _filePath unordered_mapのキー
	/// @param _mesh 追加するmesh
	void AddMesh(const std::string& _filePath, std::unique_ptr<Mesh>& _mesh);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<GraphicsResourceLoader>                resourceLoader_;


	/// mesh
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes_;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief meshの取得
	/// @param _index vectorのindex
	/// @return mesh
	const Mesh* GetMesh(const std::string& _filePath) const {
		auto itr = meshes_.find(_filePath);
		if (itr == meshes_.end()) {
			return nullptr;
		}

		return itr->second.get();
	}

};

