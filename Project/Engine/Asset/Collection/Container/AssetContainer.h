#pragma once

/// std
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Asset/Meta/MetaFile.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// アセットのインターフェイスクラス
/// ///////////////////////////////////////////////////
class IAssetContainer {
public:
	virtual ~IAssetContainer() = default;
};


/// ///////////////////////////////////////////////////
/// リソースのコンテナクラス
/// ///////////////////////////////////////////////////
template <typename T>
class AssetContainer : public IAssetContainer {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AssetContainer(size_t _maxResourceSize);
	~AssetContainer();

	/// 追加
	T* Add(const std::string& _key, T _t);

	/// 削除
	void Remove(const std::string& _key);
	void Remove(int32_t _index);


	/// --------------- 取得用 --------------- ///

	/// @brief stringのキーから取得
	/// @param _key mapのキー
	/// @return 見つかったらポインタ、見つからなかったらnullptr
	T* Get(const std::string& _key);

	/// @brief 
	/// @param _index 
	/// @return 
	T* Get(int32_t _index);

	/// @brief 先頭の要素を返す
	/// @return 先頭の要素へのポインタ
	T* GetFirst();


	/// @brief vectorのインデックスからキーを取得
	/// @param _index 配列のインデックス
	/// @return _indexに対応するキー文字列
	const std::string& GetKey(int32_t _index) const;
	

	/// @brief mapのキーからvectorのインデックスを取得
	/// @param _key マップのキー
	/// @return _keyに対応する配列のインデックス
	int32_t GetIndex(const std::string& _key) const;

	/// @brief Guidからvectorのインデックスを取得
	/// @param _guid 要素のGuid
	/// @return 存在すればインデックス、存在しなければ-1
	int32_t GetIndex(const Guid& _guid) const;


	/// @brief すべての要素を取得
	const std::vector<T>& GetValues() const;
	std::vector<T>& GetValues();
	

	/// @brief すべてのIndexMapを取得
	const std::unordered_map<std::string, int32_t>& GetIndexMap() const;


	/// @brief mapのキーからGuidを取得
	/// @param _key マップのキー
	/// @return 見つかったGuidへの参照
	const Guid& GetGuid(const std::string& _key) const;

	/// @brief vectorのインデックスからGuidを取得
	/// @param _index 配列のインデックス
	/// @return 見つかったGuidへの参照
	const Guid& GetGuid(int32_t _index) const;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, int32_t> indexMap_;
	std::unordered_map<int32_t, std::string> reverseIndexMap_;

	std::unordered_map<Guid, int32_t> guidToIndexMap_; /// Guidからvalues_のIndexを取得するためのマップ
	std::unordered_map<int32_t, Guid> indexToGuidMap_; /// values_のIndexからGuidを取得するためのマップ

	std::vector<T> values_;

};


/// ///////////////////////////////////////////////////
/// methods
/// ///////////////////////////////////////////////////

template<typename T>
inline AssetContainer<T>::AssetContainer(size_t _maxResourceSize) {
	values_.resize(_maxResourceSize);
}

template<typename T>
inline AssetContainer<T>::~AssetContainer() {}

template<typename T>
inline T* AssetContainer<T>::Add(const std::string& _key, T _t) {
	/// すでに同じキーが存在する場合は、値を更新
	if (indexMap_.contains(_key)) {
		uint32_t index = indexMap_[_key];
		values_[index] = _t;
		return &values_[index];
	}

	/// 新しいキーの場合は、値を追加
	uint32_t index = static_cast<uint32_t>(indexMap_.size());
	indexMap_[_key] = index;
	reverseIndexMap_[index] = _key;

	
	/// _keyのファイルがあるなら.metaファイルを読み込んでGuidを登録する
	if (std::filesystem::exists(_key + ".meta")) {
		MetaFile metaFile;
		metaFile.LoadFromFile(_key + ".meta");
		Guid& guid = metaFile.guid;
		guidToIndexMap_[guid] = index;
		indexToGuidMap_[index] = guid;
		_t.guid = guid;
	} else {
		/// .metaファイルがない場合は新しくMetaファイルを作成しGuidを登録、保存する
		MetaFile metaFile = GenerateMetaFile(_key);
		Guid& guid = metaFile.guid;
		guidToIndexMap_[guid] = index;
		indexToGuidMap_[index] = guid;
		_t.guid = guid;
	}

	values_[index] = _t;
	return &values_[index];
}

template<typename T>
inline void AssetContainer<T>::Remove(const std::string& _key) {

	/// 参照する方法を消して使えないようにする
	if(indexMap_.contains(_key)) {
		uint32_t index = indexMap_[_key];
		indexMap_.erase(_key);
		reverseIndexMap_.erase(index);
	}
}

template<typename T>
inline void AssetContainer<T>::Remove(int32_t _index) {

	/// 参照する方法を消して使えないようにする
	if(reverseIndexMap_.contains(_index)) {
		std::string key = reverseIndexMap_[_index];
		indexMap_.erase(key);
		reverseIndexMap_.erase(_index);
	}
}

template<typename T>
inline T* AssetContainer<T>::Get(const std::string& _key) {
	/// ----- IndexMapを_keyで参照して T型の Assetを返す ----- ///
	if (indexMap_.contains(_key)) {
		uint32_t index = indexMap_[_key];
		return &values_[index];
	}

	return nullptr;
}

template<typename T>
inline T* AssetContainer<T>::Get(int32_t _index) {
	/// ----- Indexで直接参照してアセットを返す ----- ///

	if( _index < values_.size()) {
		return &values_[_index];
	}

	return nullptr;
}

template<typename T>
inline T* AssetContainer<T>::GetFirst() {
	return &values_.front();
}

template<typename T>
inline const std::string& AssetContainer<T>::GetKey(int32_t _index) const {
	if(reverseIndexMap_.contains(_index)) {
		return reverseIndexMap_.at(_index);
	}

	/// 空文字を返す(const&で返すためstaticで用意)
	static const std::string emptyString;
	return emptyString;
}

template<typename T>
inline int32_t AssetContainer<T>::GetIndex(const std::string& _key) const {
	if(indexMap_.contains(_key)) {
		return indexMap_.at(_key);
	}

	/// 存在しない場合は無効なインデックスを返す
	return -1;
}

template<typename T>
inline int32_t AssetContainer<T>::GetIndex(const Guid& _guid) const {
	if(guidToIndexMap_.contains(_guid)) {
		return guidToIndexMap_.at(_guid);
	}

	/// 存在しない場合は無効なインデックスを返す
	return -1;
}

template<typename T>
inline const std::vector<T>& AssetContainer<T>::GetValues() const {
	return values_;
}

template<typename T>
inline std::vector<T>& AssetContainer<T>::GetValues() {
	return values_;
}

template<typename T>
inline const std::unordered_map<std::string, int32_t>& AssetContainer<T>::GetIndexMap() const {
	return indexMap_;
}

template<typename T>
inline const Guid& AssetContainer<T>::GetGuid(const std::string& _key) const {
	if (indexMap_.contains(_key)) {
		return indexToGuidMap_.at(indexMap_.at(_key));
	}

	return Guid::kInvalid;
}

template<typename T>
inline const Guid& AssetContainer<T>::GetGuid(int32_t _index) const {
	return indexToGuidMap_.at(_index);
}

} /// ONEngine
