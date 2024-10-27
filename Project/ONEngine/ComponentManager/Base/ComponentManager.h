#pragma once

/// std
#include <unordered_map>
#include <memory>

#include "ComponentArray.h"


class ComponentManager final {
public:

	void Update();

	template <typename T, typename = typename std::enable_if<std::is_base_of<BaseComponent, T>::value>::type>
	T* AddComponent() {
		size_t id = ComponentArray<T>::GetId();
		if(componentArrays_.find(id) == componentArrays_.end()) {
			componentArrays_[id] = std::make_shared<ComponentArray<T>>();
		}
		return std::static_pointer_cast<ComponentArray<CompType>>(componentArrays_[id])->AddComponent();
	}

private:

	std::unordered_map<size_t, std::shared_ptr<BaseComponentArray>> componentArrays_;
};