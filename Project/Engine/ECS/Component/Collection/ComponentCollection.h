#pragma once

/// std
#include <unordered_map>
#include <functional>

#include "../Array/ComponentArray.h"
#include "ComponentHash.h"

/// //////////////////////////////////////////////
/// Componentのコレクションクラス
/// //////////////////////////////////////////////
class ComponentCollection {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ComponentCollection();
	~ComponentCollection();


	/// @brief Componentのファクトリを登録する
	/// @tparam Comp Componentの型
	template<typename Comp>
	void RegisterComponentFactory();

	/// @brief 新規Componentを追加する
	/// @tparam Comp Componentの型
	/// @return 追加されたComponentのポインタ、失敗したら nullptr
	template<typename Comp>
	Comp* AddComponent() requires std::is_base_of_v<IComponent, Comp>;

	/// @brief 新規Componentを追加する
	/// @param _name Componentの名前
	/// @return 追加されたComponentのポインタ、失敗したら nullptr
	IComponent* AddComponent(const std::string& _name);

	/// @brief Componentを取得する
	/// @tparam Comp Componentの型
	/// @param _index Arrayのインデックス
	/// @return Componentのポインタ、失敗したら nullptr
	template<typename Comp>
	Comp* GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

	template<typename Comp>
	void RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

	void RemoveComponent(size_t _hash, size_t _id);

	void RemoveComponentAll(class GameEntity* _entity);


	template <typename Comp>
	ComponentArray<Comp>* GetComponentArray() requires std::is_base_of_v<IComponent, Comp>;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

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
		return compArray->AddComponent();
		};
}

template<typename Comp>
inline Comp* ComponentCollection::AddComponent() requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = GetComponentHash<Comp>();
	if (arrayMap_.find(hash) == arrayMap_.end()) {
		RegisterComponentFactory<Comp>();
	}

	Comp* comp = static_cast<Comp*>(factoryMap_[hash]());
	comp.id = arrayMap_[hash]->GetComponentIndex();

	return static_cast<Comp*>(factoryMap_[hash]());
}

template<typename Comp>
inline Comp* ComponentCollection::GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = GetComponentHash<Comp>();
	ComponentArray<Comp>* componentArray = static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());

	return &componentArray->components_[_index];
}

template<typename Comp>
inline void ComponentCollection::RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = GetComponentHash<Comp>();
	ComponentArray<Comp>* componentArray = static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());
	componentArray->usedIndices_.erase(std::remove(componentArray->usedIndices_.begin(), componentArray->usedIndices_.end(), _index), componentArray->usedIndices_.end());
	componentArray->removedIndices_.push_back(_index);

	componentArray->usedComponents_.erase(std::remove(
		componentArray->usedComponents_.begin(), componentArray->usedComponents_.end(),
		&componentArray->components_[_index]), componentArray->usedComponents_.end()
	);
}

template<typename Comp>
inline ComponentArray<Comp>* ComponentCollection::GetComponentArray() requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = GetComponentHash<Comp>();
	if (arrayMap_.find(hash) != arrayMap_.end()) {
		return static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());
	}
	return nullptr;
}
