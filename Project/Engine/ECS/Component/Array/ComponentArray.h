#pragma once

/// std
#include <vector>
#include <unordered_map>

/// engine
#include "../Components/Interface/IComponent.h"
#include "Engine/Core/Utility/Utility.h"

/// ///////////////////////////////////////////////////
/// Componentの配列のinterfaceクラス
/// ///////////////////////////////////////////////////
class IComponentArray {
	friend class ComponentCollection;
public:

	virtual ~IComponentArray() = default;

	virtual void RemoveComponent(size_t _index) = 0;
	virtual size_t GetComponentIndex(IComponent* _component) = 0;

protected:

	std::vector<size_t> usedIndices_;    ///< 使用中のインデックスのリスト
	std::vector<size_t> removedIndices_; ///< 削除されたインデックスのリスト
};


/// ///////////////////////////////////////////////////
/// ComponentArrayクラス
/// ///////////////////////////////////////////////////
template <typename Comp> requires std::is_base_of_v<IComponent, Comp>
class ComponentArray final : public IComponentArray {
	friend class ComponentCollection;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ComponentArray();
	~ComponentArray() override = default;

	Comp* AddComponent();

	void RemoveComponent(size_t _index) override;
	size_t GetComponentIndex(IComponent* _component) override;

	std::vector<Comp*>& GetUsedComponents();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// first: コンポーネントのID, second: インデックス
	std::unordered_map<size_t, size_t> indexMap_; ///< コンポーネントのIDとインデックスのマップ
	std::vector<Comp> components_;
	std::vector<Comp*> usedComponents_; ///< 使用中のコンポーネントのリスト
};

template<typename Comp> requires std::is_base_of_v<IComponent, Comp>
inline ComponentArray<Comp>::ComponentArray() {
	components_.reserve(1024); ///< 1024個のコンポーネントを格納できるように予約
}

template<typename Comp> requires std::is_base_of_v<IComponent, Comp>
inline Comp* ComponentArray<Comp>::AddComponent() {

	///< 削除されたインデックスがある場合
	if (removedIndices_.size() > 0) {
		size_t index = removedIndices_.back();
		removedIndices_.pop_back();
		usedIndices_.push_back(index);

		components_[index] = Comp(); ///< 今までのデータを上書き
		components_[index].id = static_cast<uint32_t>(index); ///< IDを設定

		///< IDとインデックスのマップを更新
		indexMap_[components_[index].id] = index;

		usedComponents_.push_back(&components_[index]); ///< 使用中のコンポーネントリストに追加
		return &components_[index];
	}

	components_.emplace_back();
	size_t index = components_.size() - 1;
	usedIndices_.push_back(index);

	components_[index].id = static_cast<uint32_t>(index); ///< IDを設定

	///< IDとインデックスのマップを更新
	indexMap_[components_[index].id] = index;

	usedComponents_.push_back(&components_[index]); ///< 使用中のコンポーネントリストに追加
	return &components_[index];
}

template<typename Comp> requires std::is_base_of_v<IComponent, Comp>
inline void ComponentArray<Comp>::RemoveComponent(size_t _id) {
	if (_id < components_.size()) {
		Console::Log("ComponentArray: RemoveComponent failed, index out of range.");
		return;
	}

	usedIndices_.erase(std::remove(usedIndices_.begin(), usedIndices_.end(), _id), usedIndices_.end());
	removedIndices_.push_back(_id);
	usedComponents_.erase(std::remove(usedComponents_.begin(), usedComponents_.end(), &components_[_id]), usedComponents_.end());
}

template<typename Comp> requires std::is_base_of_v<IComponent, Comp>
inline size_t ComponentArray<Comp>::GetComponentIndex(IComponent* _component) {

	for (size_t i = 0; i < components_.size(); i++) {
		const Comp& comp = components_[i];
		if (comp.GetOwner() == _component->GetOwner()) {
			return i;
		}
	}

	return 0;
}

template<typename Comp> requires std::is_base_of_v<IComponent, Comp>
inline std::vector<Comp*>& ComponentArray<Comp>::GetUsedComponents() {
	return usedComponents_;
}