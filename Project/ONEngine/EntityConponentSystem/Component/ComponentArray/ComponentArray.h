#pragma once

/// std
#include <vector>

/// component
#include "../IComponent/IComponent.h"
#include "IComponentArray.h"


/// ===================================================
/// コンポーネントを配列で管理するクラス
/// ===================================================
template <typename T>
class ComponentArray : public IComponentArray {
public:
	// TがIComponentを継承していない場合、コンパイルエラーを発生させる
	static_assert(std::is_base_of<IComponent, T>::value, "T must inherit from IComponent");

	/// ===================================================
	/// public : methods
	/// ===================================================

	ComponentArray() {}
	~ComponentArray() {}

	void Update() override;

	T* AddComponent();


	static const size_t GetId();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<T> componentArray_;
	static size_t sId_;
};


template<typename T>
size_t ComponentArray<T>::sId_ = 0;


/// ===================================================
/// public : methods
/// ===================================================

template<typename T>
inline void ComponentArray<T>::Update() {
	for(auto&& comp : componentArray_) {
		comp->Update();
	}
}

template<typename T>
inline T* ComponentArray<T>::AddComponent() {
	componentArray_.emplace_back();
	return &componentArray_.back();
}

template<typename T>
inline const size_t ComponentArray<T>::GetId() {
	if(!ComponentArray<T>::sId_) {
		ComponentArray<T>::sId_ = ++sNextCompId_;
	} 
	return ComponentArray<T>::sId_;
}
