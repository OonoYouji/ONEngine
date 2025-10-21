#pragma once

/// std
#include <memory>
#include <unordered_map>

/// engine
#include "Engine/Asset/Collection/Container/ResourceContainer.h"
#include "Engine/Asset/Collection/Loader/AssetLoader.h"

#include "Engine/Asset/Assets/Mesh/Model.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Asset/Assets/AudioClip/AudioClip.h"



static const uint32_t MAX_TEXTURE_COUNT   = 256; ///< 最大テクスチャ数
static const uint32_t MAX_MODEL_COUNT     = 128; ///< 最大モデル数
static const uint32_t MAX_AUDIOCLIP_COUNT = 128; ///< 最大オーディオクリップ数

/// ///////////////////////////////////////////////////
/// グラフィクスリソースのコレクション
/// ///////////////////////////////////////////////////
class AssetCollection final {
public:
	/// ===================================================
	/// public : sub class, enum
	/// ===================================================

	/// @brief resourceの種類
	enum class Type {
		none,
		texture,
		model,
		audio,
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AssetCollection();
	~AssetCollection();

	/// @brief 初期化関数
	/// @param _dxManager DxManagerのポインタ
	void Initialize(class DxManager* _dxManager);

	/// 読み込み
	void Load(const std::string& _filepath, Type _type);
	void LoadResources(const std::vector<std::string>& _filepaths);

	/// アンロード
	void UnloadResources(const std::vector<std::string>& _filepaths);

	/// ホットリロード
	void HotReloadAll();
	void HotReload(const std::string& _filepath);

	/// リソースの追加
	void AddModel(const std::string& _filepath, Model&& _model);
	void AddTexture(const std::string& _filepath, Texture&& _texture);
	void AddAudioClip(const std::string& _filepath, AudioClip&& _audioClip);

	/// リソースパスの取得
	std::vector<std::string> GetResourceFilePaths(const std::string& _directoryPath) const;

private:
	/// ==================================================
	/// private : methods
	/// ==================================================
	
	/// リソースのタイプを登録
	void RegisterResourceType();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<AssetLoader> resourceLoader_;

	/// リソースのコンテナ
	std::unique_ptr<ResourceContainer<Model>>     modelContainer_;
	std::unique_ptr<ResourceContainer<Texture>>   textureContainer_;
	std::unique_ptr<ResourceContainer<AudioClip>> audioClipContainer_;

	/// リソースを読む条件
	std::unordered_map<std::string, Type> resourceTypes_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ゲッタ モデル
	const Model* GetModel(const std::string& _filepath) const;
	Model* GetModel(const std::string& _filepath);

	/// ゲッタ テクスチャ
	const Texture* GetTexture(const std::string& _filepath) const;
	Texture* GetTexture(const std::string& _filepath);
	size_t GetTextureIndex(const std::string& _filepath) const;
	const std::string& GetTexturePath(size_t _index) const;
	const std::vector<Texture>& GetTextures() const;
	/// GuidからテクスチャのIndexを取得
	int32_t GetTextureIndexFromGuid(const Guid& _guid) const;

	/// ゲッタ オーディオクリップ
	const AudioClip* GetAudioClip(const std::string& _filepath) const;
	AudioClip* GetAudioClip(const std::string& _filepath);

};

