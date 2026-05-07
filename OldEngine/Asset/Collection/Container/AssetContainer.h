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
class IAssetContainer {
public:
	virtual ~IAssetContainer() = default;
};

/// ///////////////////////////////////////////////////
/// リソースのコンテナクラス
/// ///////////////////////////////////////////////////
template <IsAsset T>
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

	T* Get(const std::string& _key);
	T* Get(int32_t _index);
	T* GetFirst();

	const std::string& GetKey(int32_t _index) const;

	int32_t GetIndex(const std::string& _key) const;
	int32_t GetIndex(const Guid& _guid) const;

	const std::vector<T>& GetValues() const;
	std::vector<T>& GetValues();

	const std::unordered_map<std::string, int32_t>& GetIndexMap() const;

	const Guid& GetGuid(const std::string& _key) const;
	const Guid& GetGuid(int32_t _index) const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	mutable std::shared_mutex mtx_;

	std::unordered_map<std::string, int32_t> indexMap_;
	std::unordered_map<int32_t, std::string> reverseIndexMap_;

	std::unordered_map<Guid, int32_t> guidToIndexMap_;
	std::unordered_map<int32_t, Guid> indexToGuidMap_;

	std::vector<T> values_;
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