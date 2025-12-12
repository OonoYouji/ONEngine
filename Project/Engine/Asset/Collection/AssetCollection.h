#pragma once

/// std
#include <memory>
#include <unordered_map>

/// engine
#include "Engine/Asset/AssetType.h"
#include "Engine/Asset/Collection/Container/AssetContainer.h"
#include "Engine/Asset/Collection/Loader/AssetLoader.h"

#include "Engine/Asset/Assets/Mesh/Model.h"
#include "Engine/Asset/Assets/Texture/Texture.h"
#include "Engine/Asset/Assets/AudioClip/AudioClip.h"
#include "Engine/Asset/Assets/Mateiral/Material.h"


/// @brief TがIAssetを継承しているかのコンセプト
namespace ONEngine {

static const uint32_t MAX_TEXTURE_COUNT   = 2048; ///< 最大テクスチャ数
static const uint32_t MAX_MODEL_COUNT     = 128; ///< 最大モデル数
static const uint32_t MAX_AUDIOCLIP_COUNT = 128; ///< 最大オーディオクリップ数
static const uint32_t MAX_MATERIAL_COUNT  = 128; ///< 最大マテリアル数

/// ///////////////////////////////////////////////////
/// グラフィクスリソースのコレクション
/// ///////////////////////////////////////////////////
class AssetCollection final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AssetCollection();
	~AssetCollection();

	/// @brief 初期化関数
	/// @param _dxm DxManagerのポインタ
	void Initialize(class DxManager* _dxm);

	/// 読み込み
	void Load(const std::string& _filepath, AssetType _type);
	void LoadResources(const std::vector<std::string>& _filepaths);

	/// アンロード
	void UnloadResources(const std::vector<std::string>& _filepaths);
	void UnloadAssetByPath(const std::string& _filepath);

	/// @brief アセットの追加
	/// @tparam T 追加するアセットの型
	/// @param _filepath アセットへのファイルパス
	/// @param _asset 追加するアセットのインスタンス
	template <IsAsset T>
	void AddAsset(const std::string& _filepath, T&& _asset);

	/// @brief guidがアセットの物かチェックする
	/// @param _guid Guid
	/// @return true: アセット, false: アセットではない
	bool IsAsset(const Guid& _guid) const;


	/// @brief アセットを持っているのかチェックする
	/// @param _filepath アセットのファイルパス
	/// @return true: 持っている, false: 持っていない
	bool HasAsset(const std::string& _filepath);

	/// @brief アセットのリロード
	/// @param _filepath リロード対象のアセットパス
	/// @return true: リロード成功, false: リロード失敗
	bool ReloadAsset(const std::string& _filepath);

	/// リソースパスの取得
	std::vector<std::string> GetResourceFilePaths(const std::string& _directoryPath) const;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<AssetLoader> assetLoader_;

	/// リソースのコンテナ
	std::unique_ptr<AssetContainer<Model>>     modelContainer_;
	std::unique_ptr<AssetContainer<Texture>>   textureContainer_;
	std::unique_ptr<AssetContainer<AudioClip>> audioClipContainer_;
	std::unique_ptr<AssetContainer<Material>>  materialContainer_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================


	/// @brief アセットのパスからGuidを取得する
	/// @param _filepath ファイルパス
	/// @return 取得したGuidの参照
	const Guid& GetAssetGuidFromPath(const std::string& _filepath) const;

	/// @brief Guidからアセットのタイプを取得する
	/// @param _guid AssetのGuid
	/// @return Assetのタイプ
	AssetType GetAssetTypeFromGuid(const Guid& _guid) const;


	/// ゲッタ モデル
	const Model* GetModel(const std::string& _filepath) const;
	Model* GetModel(const std::string& _filepath);


	/// --------------------------------------------------
	/// texture methods
	/// --------------------------------------------------

	/// ゲッタ テクスチャ
	const Texture* GetTexture(const std::string& _filepath) const;
	Texture* GetTexture(const std::string& _filepath);
	size_t GetTextureIndex(const std::string& _filepath) const;
	const std::string& GetTexturePath(size_t _index) const;
	const std::vector<Texture>& GetTextures() const;

	/// @brief GuidからTextureのインデックスを取得する
	/// @param _guid 探索対象のGuid
	/// @return 見つかった場合のインデックス、見つからなかった場合は無効値
	int32_t GetTextureIndexFromGuid(const Guid& _guid) const;

	/// @brief GuidからTextureのパスを取得する
	/// @param _guid 探索対象のGuid
	/// @return 見つかった場合のパス、見つからなかった場合は空文字
	const std::string& GetTexturePath(const Guid& _guid) const;

	/// @brief GuidからTextureを取得する
	/// @param _guid TextureのGuid
	/// @return Textureのポインタ、見つからなかった場合はnullptr
	Texture* GetTextureFromGuid(const Guid& _guid) const;
	



	/// ゲッタ オーディオクリップ
	const AudioClip* GetAudioClip(const std::string& _filepath) const;
	AudioClip* GetAudioClip(const std::string& _filepath);

};

} /// ONEngine
