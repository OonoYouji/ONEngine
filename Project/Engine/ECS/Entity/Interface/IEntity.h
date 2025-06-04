#pragma once

#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Variables/Variables.h"
#include "Engine/ECS/Component/Collection/ComponentHash.h"

/// ===================================================
/// エンティティインターフェース
/// ===================================================
class IEntity {
	friend class EntityComponentSystem;
	friend class EntityCollection;
public:

	IEntity();
	virtual ~IEntity() {}

	void CommonInitialize();

	virtual void Initialize() = 0;
	virtual void Update() = 0;


	/// @brief component の追加
	/// @tparam T 追加する component の型
	/// @return 追加した component のポインタ
	template <class T>
	T* AddComponent() requires std::is_base_of_v<IComponent, T>;

	/// @brief stringから component を追加する
	/// @param _name componentの名前
	/// @return 追加した component のポインタ
	IComponent* AddComponent(const std::string& _name);

	/// @brief component の取得
	/// @tparam T ゲットする component の型
	/// @return component のポインタ
	template <class T>
	T* GetComponent() const requires std::is_base_of_v<IComponent, T>;

	void RemoveComponentAll();

	void UpdateTransform();



protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	Transform* transform_;
	Variables* variables_;
	class EntityComponentSystem* pEntityComponentSystem_;

	bool active_ = true; ///< true のときは更新する
	uint32_t id_ = 0; ///< entityのID

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<size_t, IComponent*> components_;
	std::vector<IEntity*> children_;
	IEntity* parent_;
	std::string name_;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetPosition(const Vector3& _v);
	void SetPositionX(float _x);
	void SetPositionY(float _y);
	void SetPositionZ(float _z);

	void SetRotate(const Vector3& _v);
	void SetRotateX(float _x);
	void SetRotateY(float _y);
	void SetRotateZ(float _z);

	void SetScale(const Vector3& _v);
	void SetScaleX(float _x);
	void SetScaleY(float _y);
	void SetScaleZ(float _z);

	void SetParent(IEntity* _parent);
	void RemoveParent();

	void SetName(const std::string& _name);

	void SetActive(bool _active);


	const Vector3& GetLocalPosition() const;
	const Vector3& GetLocalRotate() const;
	const Vector3& GetLocalScale() const;

	Vector3 GetWorldPosition();
	Vector3 GetWorldRotate();
	Vector3 GetWorldScale();

	Transform* GetTransform() const;

	const IEntity* GetParent() const;
	IEntity* GetParent();

	const std::vector<IEntity*>& GetChildren() const;
	IEntity* GetChild(size_t _index);

	const std::unordered_map<size_t, IComponent*>& GetComponents() const;

	const std::string& GetName() const;

	bool GetActive() const;

	uint32_t GetId() const;

};

template<class T>
inline T* IEntity::AddComponent() requires std::is_base_of_v<IComponent, T> {
	std::string name = typeid(T).name();
	if (name.find("class ") == 0) {
		name = name.substr(6);
	}
	return static_cast<T*>(AddComponent(name));
}

template<class T>
inline T* IEntity::GetComponent() const requires std::is_base_of_v<IComponent, T> {
	auto it = components_.find(GetComponentHash<T>());
	if (it != components_.end()) {
		return dynamic_cast<T*>(it->second);
	}
	return nullptr;
}
