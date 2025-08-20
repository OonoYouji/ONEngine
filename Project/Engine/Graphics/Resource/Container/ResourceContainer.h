#pragma once

/// std
#include <string>
#include <vector>
#include <unordered_map>

/// モデル、テクスチャ、音、シェーダー の4種類


/// ///////////////////////////////////////////////////
/// リソースのインターフェイスクラス
/// ///////////////////////////////////////////////////
class IResourceContainer {

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

	ResourceContainer();
	~ResourceContainer();

	/// 追加
	T Add(const std::string& _key, T _t);

	/// 削除
	void Remove(const std::string& _key);
	void Remove(size_t _index);

	/// 取得
	T* Get(const std::string& _key);
	T* Get(size_t _index);
	T* GetFirst();
	const std::string& GetKey(size_t _index) const;
	const std::vector<T>& GetValues() const;
	std::vector<T>& GetValues();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, size_t> indexMap_;
	std::unordered_map<size_t, std::string> reverseIndexMap_;
	std::vector<T> values_;

};


/// ///////////////////////////////////////////////////
/// methods
/// ///////////////////////////////////////////////////

template<typename T>
inline ResourceContainer<T>::ResourceContainer() {}

template<typename T>
inline ResourceContainer<T>::~ResourceContainer() {}

template<typename T>
inline T ResourceContainer<T>::Add(const std::string& _key, T _t) {
	/// すでに同じキーが存在する場合は、値を更新
	if (indexMap_.contains(_key)) {
		size_t index = indexMap_[_key];
		values_[index] = _t;
		return values_[index];
	}

	/// 新しいキーの場合は、値を追加
	indexMap_[_key] = values_.size();
	reverseIndexMap_[values_.size()] = _key;
	values_.emplace_back(_t);
	return values_.back();
}

template<typename T>
inline void ResourceContainer<T>::Remove(const std::string& _key) {

	/// 参照する方法を消して使えないようにする
	if(indexMap_.contains(_key)) {
		size_t index = indexMap_[_key];
		indexMap_.erase(_key);
		reverseIndexMap_.erase(index);
	}
}

template<typename T>
inline void ResourceContainer<T>::Remove(size_t _index) {

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
		size_t index = indexMap_[_key];
		return &values_[index];
	}

	return nullptr;
}

template<typename T>
inline T* ResourceContainer<T>::Get(size_t _index) {
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
inline const std::string& ResourceContainer<T>::GetKey(size_t _index) const {
	if(reverseIndexMap_.contains(_index)) {
		return reverseIndexMap_.at(_index);
	}

	/// 空文字を返す
	return std::string();
}

template<typename T>
inline const std::vector<T>& ResourceContainer<T>::GetValues() const {
	return values_;
}

template<typename T>
inline std::vector<T>& ResourceContainer<T>::GetValues() {
	return values_;
}
