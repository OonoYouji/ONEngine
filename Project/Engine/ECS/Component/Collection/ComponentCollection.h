#pragma once

#include "../Array/ComponentArray.h"
#include "ComponentHash.h"

class ComponentCollection {
public:

	ComponentCollection();
	~ComponentCollection();

	template<typename Comp>
	void RegisterComponentFactory();

	template<typename Comp>
	Comp* AddComponent() requires std::is_base_of_v<IComponent, Comp>;

	IComponent* AddComponent(const std::string& _name);

	template<typename Comp>
	Comp* GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

	template<typename Comp>
	void RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

private:
	std::unordered_map<size_t, std::unique_ptr<IComponentArray>> arrayMap_;
	std::unordered_map<size_t, std::function<IComponent* ()>> factoryMap_;
};


/// //////////////////////////////////////////////
/// inline methods
/// //////////////////////////////////////////////

template<typename Comp>
inline void ComponentCollection::RegisterComponentFactory() {
	size_t hash = GetComponentHash<Comp>();
	if (arrayMap_.find(hash) == arrayMap_.end()) {
		arrayMap_[hash] = std::make_unique<ComponentArray<Comp>>();
	}

	factoryMap_[hash] = [this, hash]() -> IComponent* {
		ComponentArray<Comp>* compArray = static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());
		if (compArray->removedIndices_.size() > 0) { ///< 削除されたインデックスがある場合
			size_t index = compArray->removedIndices_.back();
			compArray->removedIndices_.pop_back();
			compArray->usedIndices_.push_back(index);
			compArray->components_[index] = Comp(); ///< 今までのデータを上書き
			return &compArray->components_[index];
		}

		compArray->components_.emplace_back();
		size_t index = compArray->components_.size() - 1;
		compArray->usedIndices_.push_back(index);
		
		return &compArray->components_[index];
	};
}

template<typename Comp>
inline Comp* ComponentCollection::AddComponent() requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = GetComponentHash<Comp>();
	if (arrayMap_.find(hash) == arrayMap_.end()) {
		RegisterComponentFactory<Comp>();
	}
	return static_cast<Comp*>(factoryMap_[hash]());
}

template<typename Comp>
inline Comp* ComponentCollection::GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = GetComponentHash<Comp>();
	return &arrayMap_[hash]->components_[_index];
}

template<typename Comp>
inline void ComponentCollection::RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = GetComponentHash<Comp>();
	ComponentArray<Comp>* componentArray = static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());
	componentArray->usedIndices_.erase(std::remove(componentArray->usedIndices_.begin(), componentArray->usedIndices_.end(), _index), componentArray->usedIndices_.end());
	componentArray->removedIndices_.push_back(_index);
}
