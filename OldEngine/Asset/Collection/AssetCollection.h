#pragma once

/// std
#include <memory>
#include <unordered_map>
#include <optional>
#include <future>
#include <vector>

/// engine
#include "Engine/Asset/AssetType.h"
#include "Engine/Asset/Collection/Container/AssetContainer.h"
#include "AssetBundle.h"

#include "Engine/Asset/Assets/Mesh/ModelLoader.h"
#include "Engine/Asset/Assets/Texture/TextureLoader.h"
#include "Engine/Asset/Assets/AudioClip/AudioClipLoader.h"
#include "Engine/Asset/Assets/Material/MaterialLoader.h"
#include "Engine/Asset/Assets/Shader/ShaderLoader.h"
#include "Engine/Asset/Guid/Guid.h"

namespace ONEngine {
class DxManager;
}


namespace ONEngine::Asset{

static const uint32_t MAX_TEXTURE_COUNT   = 2048; ///< 最大テクスチャ数
static const uint32_t MAX_MODEL_COUNT     = 128;  ///< 最大モデル数
static const uint32_t MAX_AUDIOCLIP_COUNT = 128;  ///< 最大オーディオクリップ数
static const uint32_t MAX_MATERIAL_COUNT  = 128;  ///< 最大マテリアル数

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
	void Initialize(DxManager* dxm);

	/// 読み込み
	void Load(const std::string& filepath, AssetType type);
	void LoadResources(const std::vector<std::string>& filepaths);

	void LoadResourcesAsync(const std::vector<std::string>& filepaths);
	void WaitAllLoads();

	/// アンロード
	void UnloadResources(const std::vector<std::string>& filepaths);
	void UnloadAssetByPath(const std::string& filepath);


	/// @brief アセットを取得する
	/// @tparam T 取得したアセットの型
	/// @param _guid アセットのGuid
	/// @return 所得できたアセットのポインタ、見つからなかった場合はnullptr
	template <IsAsset T>
	const T* GetAsset(const Guid& guid) const;

	/// @brief アセットのパスを取得する
	/// @tparam T 取得したアセットの型
	/// @param _guid アセットのGuid
	/// @return 取得出来たアセットのパスの参照
	template <IsAsset T>
	const std::string& GetAssetPath(const Guid& guid) const;


	/// @brief アセットの追加
	/// @tparam T 追加するアセットの型
	/// @param _filepath アセットへのファイルパス
	/// @param _asset 追加するアセットのインスタンス
	template <IsAsset T>
	void AddAsset(const std::string& filepath, T&& asset);

	/// @brief guidがアセットの物かチェックする
	/// @param _guid Guid
	/// @return true: アセット, false: アセットではない
	bool IsAsset(const Guid& guid) const;


	/// @brief アセットを持っているのかチェックする
	/// @param _filepath アセットのファイルパス
	/// @return true: 持っている, false: 持っていない
	bool HasAsset(const std::string& filepath);

	/// @brief アセットのリロード
	/// @param _filepath リロード対象のアセットパス
	/// @return true: リロード成功, false: リロード失敗
	bool ReloadAsset(const std::string& filepath);

	/// リソースパスの取得
	std::vector<std::string> GetResourceFilePaths(const std::string& directoryPath) const;


private:

	template <typename T>
	AssetBundle<T>* GetBundle(AssetType type) const {
		// Noneの場合は即座に無効値を返す
		if(type == AssetType::None) {
			return nullptr;
		}
		// 範囲外チェック、または初期化されていない(nullptr)チェック
		size_t index = static_cast<size_t>(type);
		if(index >= assetBundles_.size() || !assetBundles_[index]) {
			return nullptr;
		}
		return static_cast<AssetBundle<T>*>(assetBundles_[index].get());
	}

	IAssetBundle* GetBaseBundle(AssetType type) const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<IAssetBundle>> assetBundles_;

	std::vector<std::future<void>> pendingTasks_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief アセットのパスからGuidを取得する
	/// @param _filepath ファイルパス
	/// @return 取得したGuidの参照
	const Guid& GetAssetGuidFromPath(const std::string& filepath) const;

	/// @brief Guidからアセットのタイプを取得する
	/// @param _guid AssetのGuid
	/// @return Assetのタイプ
	AssetType GetAssetTypeFromGuid(const Guid& guid) const;


	/// ゲッタ モデル
	const Model* GetModel(const std::string& filepath) const;
	Model* GetModel(const std::string& filepath);


	/// --------------------------------------------------
	/// texture methods
	/// --------------------------------------------------

	/// ゲッタ テクスチャ
	const Texture* GetTexture(const std::string& filepath) const;
	Texture* GetTexture(const std::string& filepath);
	int32_t GetTextureIndex(const std::string& filepath) const;
	const std::string& GetTexturePath(size_t index) const;
	const std::vector<Texture>& GetTextures() const;

	/// @brief GuidからTextureのインデックスを取得する
	/// @param _guid 探索対象のGuid
	/// @return 見つかった場合のインデックス、見つからなかった場合は無効値
	int32_t GetTextureIndexFromGuid(const Guid& guid) const;

	/// @brief GuidからTextureのパスを取得する
	/// @param _guid 探索対象のGuid
	/// @return 見つかった場合のパス、見つからなかった場合は空文字
	const std::string& GetTexturePath(const Guid& guid) const;

	/// @brief GuidからTextureを取得する
	/// @param _guid TextureのGuid
	/// @return Textureのポインタ、見つからなかった場合はnullptr
	Texture* GetTextureFromGuid(const Guid& guid) const;


	/// ゲッタ オーディオクリップ
	const AudioClip* GetAudioClip(const std::string& filepath) const;
	AudioClip* GetAudioClip(const std::string& filepath);

};

template<IsAsset T>
inline const T* AssetCollection::GetAsset(const Guid& _guid) const {
	auto* bundle = GetBundle<T>(GetAssetTypeFromGuid(_guid));
	if(!bundle) {
		return nullptr;
	}

	auto* container = bundle->container.get();
	int32_t index = container->GetIndex(_guid);
	if(index != -1) {
		return container->Get(index);
	}

	return nullptr;
}

template<IsAsset T>
inline const std::string& AssetCollection::GetAssetPath(const Guid& _guid) const {
	auto* bundle = GetBundle<T>(GetAssetTypeFromGuid(_guid));
	if(!bundle) {
		static std::string emptyString = "";
		return emptyString;
	}

	auto* container = bundle->container.get();
	int32_t index = container->GetIndex(_guid);
	return container->GetKey(index);
}


} /// namespace ONEngine::Asset