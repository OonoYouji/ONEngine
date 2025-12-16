#pragma once

/// std
#include <unordered_map>
#include <functional>

#include "../Array/ComponentArray.h"
#include "ComponentHash.h"

/// //////////////////////////////////////////////
/// Componentのコレクションクラス
/// //////////////////////////////////////////////
namespace ONEngine {

class ComponentCollection {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ComponentCollection();
	~ComponentCollection();


	/// @brief Componentのファクトリを登録する
	/// @tparam Comp Componentの型
	template<IsComponent Comp>
	void RegisterComponentFactory();

	/// @brief 新規Componentを追加する
	/// @tparam Comp Componentの型
	/// @return 追加されたComponentのポインタ、失敗したら nullptr
	template<IsComponent Comp>
	Comp* AddComponent();

	/// @brief 新規Componentを追加する
	/// @param _name Componentの名前
	/// @return 追加されたComponentのポインタ、失敗したら nullptr
	IComponent* AddComponent(const std::string& _name);

	/// @brief Componentを取得する
	/// @tparam Comp Componentの型
	/// @param _index Arrayのインデックス
	/// @return Componentのポインタ、失敗したら nullptr
	template<IsComponent Comp>
	Comp* GetComponent(size_t _index);

	/// @brief Componentの削除
	/// @tparam Comp 削除するComponentの型
	/// @param _index Arrayのインデックス
	template<IsComponent Comp>
	void RemoveComponent(size_t _index);

	/// @brief Componentの削除
	/// @param _hash CompのHash
	/// @param _id ArrayのIndex
	void RemoveComponent(size_t _hash, size_t _id);

	/// @brief _entityのComponentをすべて削除する
	/// @param _entity 削除対象のEntity
	void RemoveComponentAll(class GameEntity* _entity);


	/// @brief Componentの配列を取得する
	/// @tparam Comp Componentの型
	/// @return ComponentArray
	template <IsComponent Comp>
	ComponentArray<Comp>* GetComponentArray();

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

template<IsComponent Comp>
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

template<IsComponent Comp>
inline Comp* ComponentCollection::AddComponent() {
	size_t hash = GetComponentHash<Comp>();
	if (arrayMap_.find(hash) == arrayMap_.end()) {
		RegisterComponentFactory<Comp>();
	}

	Comp* comp = static_cast<Comp*>(factoryMap_[hash]());
	comp.id = arrayMap_[hash]->GetComponentIndex();

	return static_cast<Comp*>(factoryMap_[hash]());
}

template<IsComponent Comp>
inline Comp* ComponentCollection::GetComponent(size_t _index) {
	size_t hash = GetComponentHash<Comp>();
	ComponentArray<Comp>* componentArray = static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());

	return &componentArray->components_[_index];
}

template<IsComponent Comp>
inline void ComponentCollection::RemoveComponent(size_t _index) {
	size_t hash = GetComponentHash<Comp>();
	ComponentArray<Comp>* componentArray = static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());
	componentArray->usedIndices_.erase(std::remove(componentArray->usedIndices_.begin(), componentArray->usedIndices_.end(), _index), componentArray->usedIndices_.end());
	componentArray->removedIndices_.push_back(_index);

	componentArray->usedComponents_.erase(std::remove(
		componentArray->usedComponents_.begin(), componentArray->usedComponents_.end(),
		&componentArray->components_[_index]), componentArray->usedComponents_.end()
	);
}

template<IsComponent Comp>
inline ComponentArray<Comp>* ComponentCollection::GetComponentArray() {
	size_t hash = GetComponentHash<Comp>();
	if (arrayMap_.find(hash) != arrayMap_.end()) {
		return static_cast<ComponentArray<Comp>*>(arrayMap_[hash].get());
	}
	return nullptr;
}

} /// ONEngine
