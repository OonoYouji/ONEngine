#pragma once

/// std
#include <memory>
#include <unordered_map>

/// engine
#include "Loader/GraphicsResourceLoader.h"
#include "ResourceData/Model.h"
#include "ResourceData/Texture.h"
#include "Container/ResourceContainer.h"

static const uint32_t MAX_TEXTURE_COUNT = 128; ///< 最大テクスチャ数
static const uint32_t MAX_MODEL_COUNT   = 128;   ///< 最大モデル数

/// ///////////////////////////////////////////////////
/// グラフィクスリソースのコレクション
/// ///////////////////////////////////////////////////
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

	/// @brief リソースのホットリロード
	/// @param _filePath 対象のファイルパス
	void HotReload(const std::string& _filePath);

	/// @brief すべてのリソースをホットリロード
	void HotReloadAll();

	/// @brief 新しい model の追加
	/// @param _filePath unordered_mapのキー
	/// @param _model 追加する model
	void AddModel(const std::string& _filePath, Model&& _model);

	/// @brief 新しい texture の追加
	/// @param _filePath unordered_mapのキー
	/// @param _texture 追加する texture
	void AddTexture(const std::string& _filePath, Texture&& _texture);


	std::vector<std::string> GetResourceFilePaths(const std::string& _directoryPath) const;


	/// 登録
	void RegisterResourceType();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<GraphicsResourceLoader> resourceLoader_;

	///// mesh
	//std::unordered_map<std::string, std::unique_ptr<Model>>   models_;
	///// texture
	//std::unordered_map<std::string, size_t> textureIndices_;
	//std::unordered_map<size_t, std::string> reverseTextureIndices_;
	//std::vector<std::unique_ptr<Texture>>   textures_;

	/// container
	using ModelContainer = ResourceContainer<Model>;
	using TextureContainer = ResourceContainer<Texture>;

	std::unique_ptr<ModelContainer> modelContainer_;
	std::unique_ptr<TextureContainer> textureContainer_;


	/// リソースを読む条件
	std::unordered_map<std::string, Type> resourceTypes_;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief modelの取得
	/// @param _filePath .slnファイルからの相対パス
	/// @return modelのポインタ
	const Model* GetModel(const std::string& _filePath) const;
	Model* GetModel(const std::string& _filePath);

	/// @brief textureの取得
	/// @param _filePath .slnファイルからの相対パス
	/// @return textureのポインタ
	const Texture* GetTexture(const std::string& _filePath) const;
	Texture* GetTexture(const std::string& _filePath);

	/// @brief textureのインデックスを取得
	/// @param _filePath .slnファイルからの相対パス
	/// @return textureのインデックス
	size_t GetTextureIndex(const std::string& _filePath) const;

	/// @brief テクスチャのパスを取得
	/// @param _index テクスチャのインデックス
	/// @return 
	const std::string& GetTexturePath(size_t _index) const;


	/// @brief textureのコンテナを取得
	/// @return texture container
	const std::vector<Texture>& GetTextures() const;
};

