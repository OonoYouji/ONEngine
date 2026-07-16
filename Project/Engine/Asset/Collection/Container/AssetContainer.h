#pragma once

/// std
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <shared_mutex> 

/// engine
#include "Engine/Asset/Assets/IAsset.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Asset/Meta/MetaFile.h"

namespace ONEngine::Asset {

/// ///////////////////////////////////////////////////
/// アセットのインターフェイスクラス
/// ///////////////////////////////////////////////////
/**
 * @class IAssetContainer
 * @brief アセットコンテナの非テンプレート基底クラス
 */
class IAssetContainer {
public:
	/**
	 * @brief デストラクタ
	 */
	virtual ~IAssetContainer() = default;
};

/**
 * @class AssetContainer
 * @brief スレッドセーフに型安全なアセットを管理するコンテナテンプレートクラス
 * @tparam T IsAsset コンセプトを満たすアセット型
 */
template <IsAsset T>
class AssetContainer : public IAssetContainer {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief 最大アセット数を指定してコンテナを初期化します。
	 * @param _maxResourceSize 最大管理リソース数
	 */
	AssetContainer(size_t _maxResourceSize);

	/**
	 * @brief デストラクタ
	 */
	~AssetContainer();

	/**
	 * @brief アセットをコンテナに追加します。既に同じキーがある場合は上書きされます。
	 * @param _key 登録キー（通常はファイルパス）
	 * @param _t 追加するアセットオブジェクト
	 * @return 追加・更新されたアセットオブジェクトへのポインタ
	 */
	T* Add(const std::string& _key, T _t);

	/**
	 * @brief 指定したキーのアセットを削除します。
	 * @param _key 削除対象のキー
	 */
	void Remove(const std::string& _key);

	/**
	 * @brief 指定したインデックスのアセットを削除します。
	 * @param _index 削除対象のインデックス
	 */
	void Remove(int32_t _index);


	/// --------------- 取得用 --------------- ///

	/**
	 * @brief 指定キーに対応するアセットへのポインタを取得します。
	 * @param _key 登録キー
	 * @return アセットオブジェクトへのポインタ（存在しない場合はnullptr）
	 */
	T* Get(const std::string& _key);

	/**
	 * @brief 指定したインデックスに対応するアセットへのポインタを取得します。
	 * @param _index インデックス
	 * @return アセットオブジェクトへのポインタ（範囲外の場合はnullptr）
	 */
	T* Get(int32_t _index);

	/**
	 * @brief コンテナの先頭のアセットを取得します。
	 * @return 先頭アセットオブジェクトへのポインタ
	 */
	T* GetFirst();

	/**
	 * @brief 指定インデックスに紐づく登録キー（パスなど）を取得します。
	 * @param _index インデックス
	 * @return キー文字列への参照
	 */
	const std::string& GetKey(int32_t _index) const;

	/**
	 * @brief 指定キーのコンテナ内インデックスを取得します。
	 * @param _key 登録キー
	 * @return インデックス（見つからない場合は -1）
	 */
	int32_t GetIndex(const std::string& _key) const;

	/**
	 * @brief 指定GUIDのアセットのコンテナ内インデックスを取得します。
	 * @param _guid 検索するGUID
	 * @return インデックス（見つからない場合は -1）
	 */
	int32_t GetIndex(const Guid& _guid) const;

	/**
	 * @brief コンテナで保持する全アセット配列を取得（読み取り専用）します。
	 * @return アセット配列の定数参照
	 */
	const std::vector<T>& GetValues() const;

	/**
	 * @brief コンテナで保持する全アセット配列を取得します。
	 * @return アセット配列の参照
	 */
	std::vector<T>& GetValues();

	/**
	 * @brief 登録キーからインデックスを引くマップを取得します。
	 * @return キーとインデックスの対応マップ定数参照
	 */
	const std::unordered_map<std::string, int32_t>& GetIndexMap() const;

	/**
	 * @brief キーに紐づくアセットのGUIDを取得します。
	 * @param _key 登録キー
	 * @return GUIDオブジェクトの定数参照
	 */
	const Guid& GetGuid(const std::string& _key) const;

	/**
	 * @brief インデックスに紐づくアセットのGUIDを取得します。
	 * @param _index インデックス
	 * @return GUIDオブジェクトの定数参照
	 */
	const Guid& GetGuid(int32_t _index) const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	mutable std::shared_mutex mtx_;                                    ///< 排他制御用の共有ミューテックス

	std::unordered_map<std::string, int32_t> indexMap_;                ///< キーからインデックスへのマップ
	std::unordered_map<int32_t, std::string> reverseIndexMap_;         ///< インデックスからキーへのマップ

	std::unordered_map<Guid, int32_t> guidToIndexMap_;                 ///< GUIDからインデックスへのマップ
	std::unordered_map<int32_t, Guid> indexToGuidMap_;                 ///< インデックスからGUIDへのマップ

	std::vector<T> values_;                                            ///< 管理される実アセットデータ配列
};

/// ///////////////////////////////////////////////////
/// methods
/// ///////////////////////////////////////////////////

template<IsAsset T>
inline AssetContainer<T>::AssetContainer(size_t _maxResourceSize) {
	values_.resize(_maxResourceSize);
}

template<IsAsset T>
inline AssetContainer<T>::~AssetContainer() {}

template<IsAsset T>
inline T* AssetContainer<T>::Add(const std::string& _key, T _t) {
	std::unique_lock<std::shared_mutex> lock(mtx_);

	if(indexMap_.contains(_key)) {
		uint32_t index = indexMap_[_key];
		values_[index] = std::move(_t);
		return &values_[index];
	}

	uint32_t index = static_cast<uint32_t>(indexMap_.size());
	indexMap_[_key] = index;
	reverseIndexMap_[index] = _key;

	guidToIndexMap_[_t.guid] = index;
	indexToGuidMap_[index] = _t.guid;

	//if(std::filesystem::exists(_key + ".meta")) {
	//	MetaFile metaFile;
	//	metaFile.LoadFromFile(_key + ".meta");
	//	Guid& guid = metaFile.guid;
	//	guidToIndexMap_[guid] = index;
	//	indexToGuidMap_[index] = guid;
	//	_t.guid = guid;
	//} else {
	//	MetaFile metaFile = GenerateMetaFile(_key);
	//	Guid& guid = metaFile.guid;
	//	guidToIndexMap_[guid] = index;
	//	indexToGuidMap_[index] = guid;
	//	_t.guid = guid;
	//}

	values_[index] = std::move(_t);
	return &values_[index];
}

template<IsAsset T>
inline void AssetContainer<T>::Remove(const std::string& _key) {
	std::unique_lock<std::shared_mutex> lock(mtx_);

	if(indexMap_.contains(_key)) {
		uint32_t index = indexMap_[_key];
		indexMap_.erase(_key);
		reverseIndexMap_.erase(index);
	}
}

template<IsAsset T>
inline void AssetContainer<T>::Remove(int32_t _index) {
	std::unique_lock<std::shared_mutex> lock(mtx_);

	if(reverseIndexMap_.contains(_index)) {
		std::string key = reverseIndexMap_[_index];
		indexMap_.erase(key);
		reverseIndexMap_.erase(_index);
	}
}

template<IsAsset T>
inline T* AssetContainer<T>::Get(const std::string& _key) {
	std::shared_lock<std::shared_mutex> lock(mtx_);

	if(indexMap_.contains(_key)) {
		uint32_t index = indexMap_[_key];
		return &values_[index];
	}
	return nullptr;
}

template<IsAsset T>
inline T* AssetContainer<T>::Get(int32_t _index) {
	std::shared_lock<std::shared_mutex> lock(mtx_);

	if(_index < values_.size()) {
		return &values_[_index];
	}
	return nullptr;
}

template<IsAsset T>
inline T* AssetContainer<T>::GetFirst() {
	std::shared_lock<std::shared_mutex> lock(mtx_);
	return &values_.front();
}

template<IsAsset T>
inline const std::string& AssetContainer<T>::GetKey(int32_t _index) const {
	std::shared_lock<std::shared_mutex> lock(mtx_);

	if(reverseIndexMap_.contains(_index)) {
		return reverseIndexMap_.at(_index);
	}
	static const std::string emptyString;
	return emptyString;
}

template<IsAsset T>
inline int32_t AssetContainer<T>::GetIndex(const std::string& _key) const {
	std::shared_lock<std::shared_mutex> lock(mtx_);

	if(indexMap_.contains(_key)) {
		return indexMap_.at(_key);
	}
	return -1;
}

template<IsAsset T>
inline int32_t AssetContainer<T>::GetIndex(const Guid& _guid) const {
	std::shared_lock<std::shared_mutex> lock(mtx_);

	if(guidToIndexMap_.contains(_guid)) {
		return guidToIndexMap_.at(_guid);
	}
	return -1;
}

template<IsAsset T>
inline const std::vector<T>& AssetContainer<T>::GetValues() const {
	std::shared_lock<std::shared_mutex> lock(mtx_);
	return values_;
}

template<IsAsset T>
inline std::vector<T>& AssetContainer<T>::GetValues() {
	std::shared_lock<std::shared_mutex> lock(mtx_);
	return values_;
}

template<IsAsset T>
inline const std::unordered_map<std::string, int32_t>& AssetContainer<T>::GetIndexMap() const {
	std::shared_lock<std::shared_mutex> lock(mtx_);
	return indexMap_;
}

template<IsAsset T>
inline const Guid& AssetContainer<T>::GetGuid(const std::string& _key) const {
	std::shared_lock<std::shared_mutex> lock(mtx_);

	if(indexMap_.contains(_key)) {
		return indexToGuidMap_.at(indexMap_.at(_key));
	}
	return Guid::kInvalid;
}

template<IsAsset T>
inline const Guid& AssetContainer<T>::GetGuid(int32_t _index) const {
	std::shared_lock<std::shared_mutex> lock(mtx_);
	return indexToGuidMap_.at(_index);
}

} /// namespace ONEngine::Asset