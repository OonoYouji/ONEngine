#pragma once

/// engine
#include "Engine/Component/IComponent.h"
#include "Engine/Component/Transform/Transform.h"

/// std
#include <unordered_map>
#include <memory>

/// ===================================================
/// エンティティインターフェース
/// ===================================================
class IEntity {
public:

	IEntity();

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

	/// @brief component の取得
	/// @tparam T ゲットする component の型
	/// @return component のポインタ
	template <class T>
	T* GetComponent() const requires std::is_base_of_v<IComponent, T>;


	/// @brief transform の取得
	/// @return transform のポインタ
	const Transform* GetTransform() const { return transform_; }


protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	Transform* transform_;


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
	component->SetOwner(this);
	T* componentPtr = component.get();
	components_.emplace(hash, std::move(component));
	return componentPtr;
}

template<class T>
inline T* IEntity::GetComponent() const requires std::is_base_of_v<IComponent, T> {
	auto it = components_.find(typeid(T).hash_code());
	if (it != components_.end()) {
		return dynamic_cast<T*>(it->second.get());
	}
	return nullptr;
}
