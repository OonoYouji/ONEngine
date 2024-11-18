#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <type_traits>

#include "../Component/IComponent/IComponent.h"
#include "../Component/ComponentManager/ComponentManager.h"

class Entity {
public:

	Entity(size_t _id);


	template<typename T, typename... Args>
	T* AddComponent(Args&&... args);

	template<typename T>
	T* GetComponent();

	size_t GetId() const { return id_; }

private:

	std::vector<IComponent*> pComponents_;
	size_t id_;
};



template<typename T, typename ...Args>
inline T* Entity::AddComponent(Args && ...args) {
	/// コンポーネントがIComponentを継承しているかをチェック
	static_assert(std::is_base_of<IComponent, T>::value, "T must inherit from BaseComponent");
	size_t id = ComponentArray<T>::GetId();

	ComponentManager* compManager = ComponentManager::GetInstance();
	T* componentPtr = compManager->AddComponent<T>();
	componentPtr->SetOwner(this);
	componentPtr->Initialize();

	if(pComponents_.size() <= id) {
		pComponents_.resize(id + 1, nullptr);
	}

	pComponents_[id] = componentPtr;
	return componentPtr;
}

template<typename T>
inline T* Entity::GetComponent() {
	size_t id = ComponentArray<T>::GetId();
	return static_cast<T*>(pComponents_[id]);
}
