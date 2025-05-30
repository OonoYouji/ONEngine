#pragma once

/// std
#include <vector>

/// engine
#include "../Components/Interface/IComponent.h"
#include "Engine/Core/Utility/Utility.h"

/// ///////////////////////////////////////////////////
/// Componentの配列のinterfaceクラス
/// ///////////////////////////////////////////////////
class IComponentArray {
	friend class EntityComponentSystem;
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
	friend class EntityComponentSystem;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ComponentArray();
	~ComponentArray() override = default;

	void RemoveComponent(size_t _index) override;
	size_t GetComponentIndex(IComponent* _component) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<Comp> components_;
};

template<typename Comp> requires std::is_base_of_v<IComponent, Comp>
inline ComponentArray<Comp>::ComponentArray() {
	components_.reserve(1024); ///< 1024個のコンポーネントを格納できるように予約
}

template<typename Comp> requires std::is_base_of_v<IComponent, Comp>
inline void ComponentArray<Comp>::RemoveComponent(size_t _index) {
	if (_index < components_.size()) {
		Console::Log("ComponentArray: RemoveComponent failed, index out of range.");
		return;
	}

	usedIndices_.erase(std::remove(usedIndices_.begin(), usedIndices_.end(), _index), usedIndices_.end());
	removedIndices_.push_back(_index);
	//components_[_index] = Comp(); ///< コンポーネントを初期化
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

