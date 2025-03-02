#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// engine
#include <Engine/Component/Transform/Transform.h>


/// ===================================================
/// エンティティインターフェース
/// ===================================================
class IEntity {
	friend class EntityCollection;
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


protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	Transform* transform_;
	class EntityCollection* entityCollection_;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<size_t, std::unique_ptr<IComponent>> components_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetPosition(const Vector3& _v) { transform_->SetPosition(_v); }
	void SetPositionX(float _x) { transform_->SetPositionX(_x); }
	void SetPositionY(float _y) { transform_->SetPositionY(_y); }
	void SetPositionZ(float _z) { transform_->SetPositionZ(_z); }

	void SetRotate(const Vector3& _v) { transform_->SetRotate(_v); }
	void SetRotateX(float _x) { transform_->SetRotateX(_x); }
	void SetRotateY(float _y) { transform_->SetRotateY(_y); }
	void SetRotateZ(float _z) { transform_->SetRotateZ(_z); }

	void SetScale(const Vector3& _v) { transform_->SetScale(_v); }
	void SetScaleX(float _x) { transform_->SetScaleX(_x); }
	void SetScaleY(float _y) { transform_->SetScaleY(_y); }
	void SetScaleZ(float _z) { transform_->SetScaleZ(_z); }

	void SetParent(IEntity* _parent);


	const Vector3& GetPosition() const { return transform_->GetPosition(); }
	const Vector3& GetRotate()   const { return transform_->GetRotate(); }
	const Vector3& GetScale()    const { return transform_->GetScale(); }

	
	/// @brief transform の取得
	/// @return transform のポインタ
	Transform* GetTransform() const { return transform_; }

};



/// ===================================================
/// inline methods
/// ===================================================

template<class T>
inline T* IEntity::AddComponent() requires std::is_base_of_v<IComponent, T> {

	size_t hash = typeid(T).hash_code();
	auto it = components_.find(hash);
	if (it != components_.end()) { ///< すでに同じコンポーネントが存在している場合
		return static_cast<T*>(it->second.get());
	}

	/// component の生成, 追加
	std::unique_ptr<T> component = std::make_unique<T>();
	component->SetOwner(this);
	T* componentPtr = component.get();
	components_.emplace(hash, std::move(component));

	return componentPtr; ///< 生成した component のポインタを返す
}

template<class T>
inline T* IEntity::GetComponent() const requires std::is_base_of_v<IComponent, T> {
	auto it = components_.find(typeid(T).hash_code());
	if (it != components_.end()) {
		return dynamic_cast<T*>(it->second.get());
	}
	return nullptr;
}
