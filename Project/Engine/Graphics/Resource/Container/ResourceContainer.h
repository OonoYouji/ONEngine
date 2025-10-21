#pragma once

/// std
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>

/// engine
#include "Engine/Resource/Guid/Guid.h"
#include "Engine/Resource/Meta/MetaFile.h"

/// ///////////////////////////////////////////////////
/// リソースのインターフェイスクラス
/// ///////////////////////////////////////////////////
class IResourceContainer {
public:
	virtual ~IResourceContainer() = default;
};


/// ///////////////////////////////////////////////////
/// リソースのコンテナクラス
/// ///////////////////////////////////////////////////
template <typename T>
class ResourceContainer : public IResourceContainer {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ResourceContainer(size_t _maxResourceSize);
	~ResourceContainer();

	/// 追加
	T* Add(const std::string& _key, T _t);

	/// 削除
	void Remove(const std::string& _key);
	void Remove(int32_t _index);

	/// 取得
	T* Get(const std::string& _key);
	T* Get(int32_t _index);
	T* GetFirst();
	const std::string& GetKey(int32_t _index) const;
	int32_t GetIndex(const std::string& _key) const;
	int32_t GetIndex(const Guid& _guid) const;
	const std::vector<T>& GetValues() const;
	std::vector<T>& GetValues();
	const std::unordered_map<std::string, int32_t>& GetIndexMap() const;

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
inline ResourceContainer<T>::ResourceContainer(size_t _maxResourceSize) {
	values_.resize(_maxResourceSize);
}

template<typename T>
inline ResourceContainer<T>::~ResourceContainer() {}

template<typename T>
inline T* ResourceContainer<T>::Add(const std::string& _key, T _t) {
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
	values_[index] = _t;

	
	/// _keyのファイルがあるなら.metaファイルを読み込んでGuidを登録する
	if (std::filesystem::exists(_key + ".meta")) {
		MetaFile metaFile;
		metaFile.LoadFromFile(_key + ".meta");
		Guid& guid = metaFile.guid;
		guidToIndexMap_[guid] = index;
		indexToGuidMap_[index] = guid;
	} else {
		/// .metaファイルがない場合は新しくMetaファイルを作成しGuidを登録、保存する
		MetaFile metaFile = GenerateMetaFile(_key);
		Guid& guid = metaFile.guid;
		guidToIndexMap_[guid] = index;
		indexToGuidMap_[index] = guid;
	}

	return &values_[index];
}

template<typename T>
inline void ResourceContainer<T>::Remove(const std::string& _key) {

	/// 参照する方法を消して使えないようにする
	if(indexMap_.contains(_key)) {
		uint32_t index = indexMap_[_key];
		indexMap_.erase(_key);
		reverseIndexMap_.erase(index);
	}
}

template<typename T>
inline void ResourceContainer<T>::Remove(int32_t _index) {

	/// 参照する方法を消して使えないようにする
	if(reverseIndexMap_.contains(_index)) {
		std::string key = reverseIndexMap_[_index];
		indexMap_.erase(key);
		reverseIndexMap_.erase(_index);
	}
}

template<typename T>
inline T* ResourceContainer<T>::Get(const std::string& _key) {
	if (indexMap_.contains(_key)) {
		uint32_t index = indexMap_[_key];
		return &values_[index];
	}

	return nullptr;
}

template<typename T>
inline T* ResourceContainer<T>::Get(int32_t _index) {
	if( _index < values_.size()) {
		return &values_[_index];
	}

	return nullptr;
}

template<typename T>
inline T* ResourceContainer<T>::GetFirst() {
	return &values_.front();
}

template<typename T>
inline const std::string& ResourceContainer<T>::GetKey(int32_t _index) const {
	if(reverseIndexMap_.contains(_index)) {
		return reverseIndexMap_.at(_index);
	}

	/// 空文字を返す
	return std::string();
}

template<typename T>
inline int32_t ResourceContainer<T>::GetIndex(const std::string& _key) const {
	if(indexMap_.contains(_key)) {
		return indexMap_.at(_key);
	}

	/// 存在しない場合は無効なインデックスを返す
	return -1;
}

template<typename T>
inline int32_t ResourceContainer<T>::GetIndex(const Guid& _guid) const {
	if(guidToIndexMap_.contains(_guid)) {
		return guidToIndexMap_.at(_guid);
	}

	return -1;
}

template<typename T>
inline const std::vector<T>& ResourceContainer<T>::GetValues() const {
	return values_;
}

template<typename T>
inline std::vector<T>& ResourceContainer<T>::GetValues() {
	return values_;
}

template<typename T>
inline const std::unordered_map<std::string, int32_t>& ResourceContainer<T>::GetIndexMap() const {
	return indexMap_;
}
