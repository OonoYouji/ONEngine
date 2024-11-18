#pragma once

/// std
#include <unordered_map>
#include <typeindex>
#include <memory>

#include "../ComponentArray/IComponentArray.h"
#include "../ComponentArray/ComponentArray.h"


/// ===================================================
/// すべてのコンポーネントの管理クラス
/// ===================================================
class ComponentManager final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================


	ComponentManager() {}
	~ComponentManager() {}

	static ComponentManager* GetInstance() {
		static ComponentManager instance;
		return &instance;
	}


	void Initialize();
	void Update();


	template<typename T>
	T* AddComponent();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


	std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentArrays_;

};

template<typename T>
inline T* ComponentManager::AddComponent() {
	size_t id = ComponentArray<T>::GetId();
	if(componentArrays_.find(id) == componentArrays_.end()) {
		componentArrays_[id] = std::make_unique<ComponentArray<T>>();
	}

	return std::static_pointer_cast<ComponentArray<T>>(componentArrays_[id])->AddComponent();
}
