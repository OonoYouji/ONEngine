#pragma once

/// std
#include <vector>

/// other
#include "BaseComponent.h"

static size_t nextComponentId_ = 0;


class BaseComponentArray {
public:
	virtual void Update() {}
};


template <typename T, typename = typename std::enable_if<std::is_base_of<BaseComponent, T>::value>::type>
class ComponentArray : public BaseComponentArray {
public:

	ComponentArray() {
		components_.reserve(128);
	}

	~ComponentArray() {}


	/// <summary>
	/// 管理を任されているコンポーネントの更新処理
	/// </summary>
	void Update() override {
		for(auto&& comp : components_) {
			comp->Update();
		}
	}

	/// <summary>
	/// コンポーネントの追加
	/// </summary>
	/// <returns> return : 追加されたコンポーネントへのポインタ </returns>
	T* AddComponent() {
		components_.emplace_back();
		return &components_.back();
	}

	/// <summary>
	/// 管理をしているコンポーネントのidをゲットする
	/// </summary>
	/// <returns> return : 管理しているコンポーネントのID </returns>
	static const size_t GetId() {
		if(!ComponentArray<T>::componentId_) {
			ComponentArray<T>::componentId_ = ++nextComponentId_;
		} 
		return ComponentArray<T>::componentId_;
	}

private:
	static size_t componentId_; /// このコンテナが管理するコンポーネントの一意なID
	std::vector<T> components_; /// component array
};