#pragma once

/// std
#include <memory>
#include <future>

/// engine
#include "Engine/Asset/Assets/IAsset.h"
#include "Engine/Asset/Assets/IAssetLoader.h"
#include "Engine/Asset/Collection/Container/AssetContainer.h"
#include "Engine/Core/Threading/ThreadPool.h" 

namespace ONEngine::Asset {

/**
 * @class IAssetBundle
 * @brief アセットバンドル（同一型のアセットコンテナとローダーのペア）の非テンプレート抽象インターフェースクラス
 */
class IAssetBundle {
public:
	/**
	 * @brief デストラクタ
	 */
	virtual ~IAssetBundle() = default;

	/**
	 * @brief 同期的にアセットをロードします。
	 * @param _filepath 対象アセットのファイルパス
	 */
	virtual void Load(const std::string& _filepath) = 0;

	/**
	 * @brief 非同期的にアセットをスレッドプール上でロードします。
	 * @param _filepath 対象アセットのファイルパス
	 * @return ロード処理の非同期終了を待機するための std::future<void>
	 */
	virtual std::future<void> LoadAsync(const std::string& _filepath) = 0;

	/**
	 * @brief 既存アセットを再ロード（リロード）します。
	 * @param _filepath 対象アセットのファイルパス
	 */
	virtual void Reload(const std::string& _filepath) = 0;

	/**
	 * @brief アセットファイルパスに対応するGUIDを取得します。
	 * @param _filepath 対象アセットのファイルパス
	 * @return アセットのGUIDへの定数参照
	 */
	virtual const Guid& GetGuid(const std::string& _filepath) const = 0;

	/**
	 * @brief 指定アセットをコンテナからアンロード（削除）します。
	 * @param _filepath アンロード対象アセットのファイルパス
	 */
	virtual void Remove(const std::string& _filepath) = 0;

	/**
	 * @brief 指定GUIDのアセットがロード済みであるかを判定します。
	 * @param _guid 対象アセットのGUID
	 * @return ロード済みの場合はtrue、未ロードの場合はfalse
	 */
	virtual bool Contains(const Guid& _guid) const = 0;

	/**
	 * @brief 指定ファイルパスのアセットがロード済みであるかを判定します。
	 * @param _filepath 対象アセットのファイルパス
	 * @return ロード済みの場合はtrue、未ロードの場合はfalse
	 */
	virtual bool Contains(const std::string& _filepath) const = 0;
};

/**
 * @class AssetBundle
 * @brief 特定アセット型専用のアセットコンテナとローダーをペアで保持し、キャッシュや非同期ロード制御を行うクラス
 * @tparam T IsAsset コンセプトを満たすアセット型
 */
template <IsAsset T>
class AssetBundle : public IAssetBundle {
public:

	/**
	 * @brief コンストラクタ
	 */
	AssetBundle() = default;

	/**
	 * @brief デストラクタ
	 */
	~AssetBundle() override = default;

	std::unique_ptr<AssetLoader<T>> loader;       ///< 特殊化されたアセットローダー
	std::unique_ptr<AssetContainer<T>> container; ///< スレッドセーフなアセットコンテナ

	/**
	 * @brief 同期的にアセットをロードし、コンテナに追加します。
	 * @param _filepath 対象アセットのファイルパス
	 */
	void Load(const std::string& _filepath) override {

		/// キャッシュ確認
		if(container->GetIndex(_filepath) == -1) {

			/// Metaファイル読み込み
			Meta<T::MetaData> meta = loader->GetMetaData(_filepath);

			/// ロード&追加
			auto asset = loader->Load(_filepath, meta);
			if(asset.has_value()) {
				container->Add(_filepath, std::move(asset.value()));
			}
		}
	}

	/**
	 * @brief 非同期的にアセットをロードし、コンテナに追加します。
	 * @param _filepath 対象アセットのファイルパス
	 * @return std::future<void>
	 */
	std::future<void> LoadAsync(const std::string& _filepath) override {
		return ThreadPool::Instance().Enqueue([this, _filepath]() {
			if(container->GetIndex(_filepath) == -1) {
				Meta<T::MetaData> meta = loader->GetMetaData(_filepath);
				auto asset = loader->Load(_filepath, meta);
				if(asset.has_value()) {
					container->Add(_filepath, std::move(asset.value()));
				}
			}
		});
	}

	/**
	 * @brief 既存アセットを再ロード（リロード）して最新データに差し替えます。
	 * @param _filepath 対象アセットのファイルパス
	 */
	void Reload(const std::string& _filepath) override {
		int32_t index = container->GetIndex(_filepath);
		if(index != -1) {
			T* src = container->Get(index);
			Meta<T::MetaData> meta = loader->GetMetaData(_filepath);
			auto reloadedAsset = loader->Reload(_filepath, src, meta);
			if(reloadedAsset.has_value()) {
				container->Add(_filepath, std::move(reloadedAsset.value()));
			}
		}
	}

	/**
	 * @brief アセットのファイルパスからGUIDを取得します。
	 * @param _filepath 対象アセットのファイルパス
	 * @return アセットのGUIDへの定数参照
	 */
	const Guid& GetGuid(const std::string& _filepath) const override {
		return container->GetGuid(_filepath);
	}

	/**
	 * @brief 指定アセットをアンロード（削除）します。
	 * @param _filepath 対象アセットのファイルパス
	 */
	void Remove(const std::string& _filepath) override {
		container->Remove(_filepath);
	}

	/**
	 * @brief 指定GUIDのアセットがロード済みであるかを判定します。
	 * @param _guid 対象アセットのGUID
	 * @return ロード済みの場合はtrue、未ロードの場合はfalse
	 */
	bool Contains(const Guid& _guid) const override {
		return container->GetIndex(_guid) != -1;
	}

	/**
	 * @brief 指定ファイルパスのアセットがロード済みであるかを判定します。
	 * @param _filepath 対象アセットのファイルパス
	 * @return ロード済みの場合はtrue、未ロードの場合はfalse
	 */
	bool Contains(const std::string& _filepath) const override {
		return container->GetIndex(_filepath) != -1;
	}

};

} /// namespace ONEngine