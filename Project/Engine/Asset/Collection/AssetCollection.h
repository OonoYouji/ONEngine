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
#include "Engine/Asset/Assets/Animation/AnimationClipLoader.h"
#include "Engine/Asset/Guid/Guid.h"

namespace ONEngine {
class DxManager;
}


namespace ONEngine::Asset{

static const uint32_t MAX_TEXTURE_COUNT   = 2048; ///< 最大テクスチャ数
static const uint32_t MAX_MODEL_COUNT     = 128;  ///< 最大モデル数
static const uint32_t MAX_AUDIOCLIP_COUNT = 128;  ///< 最大オーディオクリップ数
static const uint32_t MAX_MATERIAL_COUNT  = 128;  ///< 最大マテリアル数

/**
 * @class AssetCollection
 * @brief 全てのアセット（テクスチャ、モデル、シェーダー、マテリアル、オーディオ、アニメーション）を一括で管理・ロード・検索するシングルトン風のコレクションクラス
 */
class AssetCollection final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	AssetCollection();
	/**
	 * @brief デストラクタ
	 */
	~AssetCollection();

	/**
	 * @brief アセットコレクションのグローバルインスタンスを取得します。
	 * @return インスタンスのポインタ
	 */
	static AssetCollection* GetInstance();

	/**
	 * @brief アセットコレクションを初期化し、各アセット型に応じたバンドルを生成します。
	 * @param dxm DirectX12デバイスを保持するマネージャ
	 */
	void Initialize(DxManager* dxm);

	/**
	 * @brief アセットタイプを指定して、単一のアセットを同期的にロードします。
	 * @param filepath アセットのファイルパス
	 * @param type アセットのタイプ
	 */
	void Load(const std::string& filepath, AssetType type);
	/**
	 * @brief 複数のアセットファイルパスのリストを受け取り、同期的に一括ロードします。
	 * @param filepaths ファイルパス配列
	 */
	void LoadResources(const std::vector<std::string>& filepaths);

	/**
	 * @brief 複数のアセットファイルパスのリストを受け取り、非同期（スレッドプール）でロードタスクを発行します。
	 * @param filepaths ファイルパス配列
	 */
	void LoadResourcesAsync(const std::vector<std::string>& filepaths);
	/**
	 * @brief 現在実行中のすべての非同期ロードタスクの完了を待機します。
	 */
	void WaitAllLoads();

	/**
	 * @brief 複数のアセットを一括アンロード（削除）します。
	 * @param filepaths 対象ファイルのパス配列
	 */
	void UnloadResources(const std::vector<std::string>& filepaths);
	/**
	 * @brief 指定パスのアセットをアンロードします。アセットの拡張子などからタイプを自動判定します。
	 * @param filepath アンロード対象のファイルパス
	 */
	void UnloadAssetByPath(const std::string& filepath);


	/**
	 * @brief GUIDからロード済みアセットを検索して取得します。
	 * @tparam T アセットの型
	 * @param guid 対象アセットのGUID
	 * @return 取得できたアセットのポインタ（見つからない場合はnullptr）
	 */
	template <IsAsset T>
	const T* GetAsset(const Guid& guid) const;

	/**
	 * @brief GUIDからアセットの登録パス（ファイルパス）を取得します。
	 * @tparam T アセットの型
	 * @param guid 対象アセットのGUID
	 * @return 登録キー文字列への定数参照（見つからない場合は空文字）
	 */
	template <IsAsset T>
	const std::string& GetAssetPath(const Guid& guid) const;


	/**
	 * @brief 手動生成されたアセットオブジェクトを直接コレクションに追加します。
	 * @tparam T アセットの型
	 * @param filepath 登録キーとして使用するダミー・または対応するファイルパス
	 * @param asset 追加するアセットオブジェクトの右辺参照
	 */
	template <IsAsset T>
	void AddAsset(const std::string& filepath, T&& asset);

	/**
	 * @brief 指定されたGUIDを持つデータがアセット（登録済み）であるかを検証します。
	 * @param guid 検証対象のGUID
	 * @return アセットである場合はtrue、そうでない場合はfalse
	 */
	bool IsAsset(const Guid& guid) const;


	/**
	 * @brief 指定パスのアセットがロード済み（キャッシュあり）であるか判定します。
	 * @param filepath アセットのファイルパス
	 * @return 保持している場合はtrue、そうでない場合はfalse
	 */
	bool HasAsset(const std::string& filepath);

	/**
	 * @brief 指定パスのアセットを再ロード（ディスクからリロード）します。
	 * @param filepath 再ロード対象のアセットパス
	 * @return リロードに成功した場合はtrue、失敗またはキャッシュにない場合はfalse
	 */
	bool ReloadAsset(const std::string& filepath);

	/**
	 * @brief 指定ディレクトリ以下のすべてのサポート対象アセット（メタファイルに紐づく拡張子）のパスリストを取得します。
	 * @param directoryPath ディレクトリパス
	 * @return サポート対象ファイルのパス配列
	 */
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

	std::vector<std::unique_ptr<IAssetBundle>> assetBundles_; ///< 各アセット型に対応したバンドルの配列

	std::vector<std::future<void>> pendingTasks_;             ///< 実行中の非同期ロードタスクリスト


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/**
	 * @brief アセットファイルパスから紐づくアセットのGUIDを取得します。
	 * @param filepath アセットのパス
	 * @return 対応するGUIDオブジェクトの定数参照（見つからない場合はGuid::kInvalid）
	 */
	const Guid& GetAssetGuidFromPath(const std::string& filepath) const;

	/**
	 * @brief 指定GUIDのアセットタイプを判別して返します。
	 * @param guid 対象アセットのGUID
	 * @return 判別されたAssetType（見つからない場合はAssetType::None）
	 */
	AssetType GetAssetTypeFromGuid(const Guid& guid) const;


	/**
	 * @brief パスを指定してModelアセットをロード済みキャッシュから取得（読み取り専用）します。
	 * @param filepath モデルのファイルパス
	 * @return Modelオブジェクトへのポインタ
	 */
	const Model* GetModel(const std::string& filepath) const;
	/**
	 * @brief パスを指定してModelアセットをロード済みキャッシュから取得します。
	 * @param filepath モデルのファイルパス
	 * @return Modelオブジェクトへのポインタ
	 */
	Model* GetModel(const std::string& filepath);


	/// --------------------------------------------------
	/// texture methods
	/// --------------------------------------------------

	/**
	 * @brief パスを指定してTextureアセットを取得（読み取り専用）します。
	 * @param filepath テクスチャのファイルパス
	 * @return Textureオブジェクトへのポインタ
	 */
	const Texture* GetTexture(const std::string& filepath) const;
	/**
	 * @brief パスを指定してTextureアセットを取得します。
	 * @param filepath テクスチャのファイルパス
	 * @return Textureオブジェクトへのポインタ
	 */
	Texture* GetTexture(const std::string& filepath);
	/**
	 * @brief 指定テクスチャパスに対応するコンテナ内インデックスを取得します。
	 * @param filepath テクスチャのパス
	 * @return インデックス（見つからない場合は -1）
	 */
	int32_t GetTextureIndex(const std::string& filepath) const;
	/**
	 * @brief ディスクリプタヒープなどで利用するテクスチャインデックスから、そのパスを取得します。
	 * @param index インデックス
	 * @return パス文字列への定数参照
	 */
	const std::string& GetTexturePath(size_t index) const;
	/**
	 * @brief コレクションされているすべてのテクスチャ配列を取得します。
	 * @return Texture配列の定数参照
	 */
	const std::vector<Texture>& GetTextures() const;

	/**
	 * @brief GUIDからTextureのコンテナ内インデックスを取得します。
	 * @param guid 検索するGUID
	 * @return インデックス（見つからない場合は -1）
	 */
	int32_t GetTextureIndexFromGuid(const Guid& guid) const;

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

	/// ゲッタ アニメーションクリップ
	const AnimationClip* GetAnimationClip(const std::string& filepath) const;
	AnimationClip* GetAnimationClip(const std::string& filepath);

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