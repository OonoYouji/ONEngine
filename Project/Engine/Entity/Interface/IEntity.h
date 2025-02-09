#pragma once

#include "Component/IComponent.h"

/// std
#include <unordered_map>
#include <memory>

/// ===================================================
/// エンティティインターフェース
/// ===================================================
class IEntity {
public:

	/// @brief 仮想デストラクタ
	virtual ~IEntity() {}

	/// @brief 初期化の純粋仮想関数
	virtual void Initialize() = 0;

	/// @brief 更新の純粋仮想関数
	virtual void Update()     = 0;


	/// @brief component の追加
	/// @tparam T 追加する component の型
	/// @return 追加した component のポインタ
	template <class T>
	T* AddComponent() requires std::is_base_of_v<IComponent, T>;

	template <class T>
	T* GetComponent() requires std::is_base_of_v<IComponent, T>;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<size_t, std::unique_ptr<IComponent>> components_;

};


/// ===================================================
/// inline methods
/// ===================================================

template<class T>
inline T* IEntity::AddComponent() requires std::is_base_of_v<IComponent, T> {

	size_t hash = typeid(T).hash_code();
	auto it = components_.find(hash);
	if (it != components_.end()) { ///< すでに同じコンポーネントが存在している場合
		return nullptr;
	}

	std::unique_ptr<T> component = std::make_unique<T>();
	T* componentPtr = component.get();
	components_.emplace(hash, std::move(component));
	return componentPtr;
}

template<class T>
inline T* IEntity::GetComponent() requires std::is_base_of_v<IComponent, T> {
	return dynamic_cast<T*>(components_[typeid(T).hash_code()].get());
}
