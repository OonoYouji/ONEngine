#pragma once

/// std
#include <vector>

/// ///////////////////////////////////////////////////
/// Componentの配列のinterfaceクラス
/// ///////////////////////////////////////////////////
class IComponentArray {};


/// ///////////////////////////////////////////////////
/// ComponentArrayクラス
/// ///////////////////////////////////////////////////
template <typename Comp>
class ComponentArray final : public IComponentArray {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ComponentArray();
	~ComponentArray() {}

	size_t AddComponent();
	Comp* GetComponent(size_t _index);
	void RemoveComponent(size_t _index);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<Comp>   components_;
	std::vector<size_t> usedIndices_;    ///< 使用中のインデックスのリスト
	std::vector<size_t> removedIndices_; ///< 削除されたインデックスのリスト
	//size_t              nextFreeIndex_ = 0; ///< 次に空いているインデックス
};

template<typename Comp>
inline ComponentArray<Comp>::ComponentArray() {
	components_.reserve(1024); ///< 1024個のコンポーネントを格納できるように予約
}

template<typename Comp>
inline size_t ComponentArray<Comp>::AddComponent() {
	if (removedIndices_.size() > 0) { ///< 削除されたインデックスがある場合
		size_t index = removedIndices_.back();
		removedIndices_.pop_back();
		usedIndices_.push_back(index);
		components_[index] = Comp(); ///< 今までのデータを上書き
		return index;
	}

	components_.emplace_back();
	size_t index = components_.size() - 1;
	usedIndices_.push_back(index);
	return index;
}

template<typename Comp>
inline Comp* ComponentArray<Comp>::GetComponent(size_t _index) {
	return &components_[_index];
}

template<typename Comp>
inline void ComponentArray<Comp>::RemoveComponent(size_t _index) {
	usedIndices_.erase(std::remove(usedIndices_.begin(), usedIndices_.end(), _index), usedIndices_.end());
	removedIndices_.push_back(_index);
}
