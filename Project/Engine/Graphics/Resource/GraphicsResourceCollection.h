#pragma once

/// std
#include <memory>
#include <unordered_map>

/// engine
#include "Loader/GraphicsResourceLoader.h"
#include "ResourceData/Model.h"


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


	/// @brief 新しい model の追加
	/// @param _filePath unordered_mapのキー
	/// @param _model 追加する model
	void AddModel(const std::string& _filePath, std::unique_ptr<Model>& _model);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<GraphicsResourceLoader>                resourceLoader_;

	/// mesh
	std::unordered_map<std::string, std::unique_ptr<Model>> models_;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief modelの取得
	/// @param _filePath .slnファイルからの相対パス
	/// @return modelのポインタ
	const Model* GetModel(const std::string& _filePath) const {
		auto itr = models_.find(_filePath);
		if (itr == models_.end()) {
			return nullptr;
		}
		return itr->second.get();
	}

};

