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
	template <class Comp>
	Comp* AddComponent() requires std::is_base_of_v<IComponent, Comp>;

	/// @brief stringから component を追加する
	/// @param _name componentの名前
	/// @return 追加した component のポインタ
	IComponent* AddComponent(const std::string& _name);

	/// @brief component の取得
	/// @tparam T ゲットする component の型
	/// @return component のポインタ
	template <class Comp>
	Comp* GetComponent() const requires std::is_base_of_v<IComponent, Comp>;

	IComponent* GetComponent(const std::string& _compName) const;

	template <typename Comp>
	void RemoveComponent() requires std::is_base_of_v<IComponent, Comp>;

	void RemoveComponent(const std::string& _compName);

	void RemoveComponentAll();

	void UpdateTransform();

	void Destroy();

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	Transform* transform_;
	Variables* variables_;
	class EntityComponentSystem* pEntityComponentSystem_;

	bool active_ = true; ///< true のときは更新する
	int32_t id_ = 0; ///< entityのID

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<size_t, IComponent*> components_;
	std::vector<IEntity*> children_;
	IEntity* parent_;
	std::string name_;
	std::string className_;
	std::string prefabName_;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetPosition(const Vector3& _v);
	void SetPositionX(float _x);
	void SetPositionY(float _y);
	void SetPositionZ(float _z);

	void SetRotate(const Vector3& _v);
	void SetRotate(const Quaternion& _q);

	void SetScale(const Vector3& _v);
	void SetScaleX(float _x);
	void SetScaleY(float _y);
	void SetScaleZ(float _z);

	void SetParent(IEntity* _parent);
	void RemoveParent();

	void SetName(const std::string& _name);
	void SetPrefabName(const std::string& _name);

	void SetActive(bool _active);


	const Vector3& GetLocalPosition() const;
	Vector3 GetLocalRotate() const;
	const Quaternion& GetLocalRotateQuaternion() const;
	const Vector3& GetLocalScale() const;

	Vector3 GetPosition();
	Vector3 GetRotate();
	Quaternion GetRotateQuaternion();
	Vector3 GetScale();

	Transform* GetTransform() const;

	const IEntity* GetParent() const;
	IEntity* GetParent();

	const std::vector<IEntity*>& GetChildren() const;
	IEntity* GetChild(size_t _index);

	const std::unordered_map<size_t, IComponent*>& GetComponents() const;
	std::unordered_map<size_t, IComponent*>& GetComponents();

	const std::string& GetName() const;
	const std::string& GetEntityClassName() const;

	const std::string& GetPrefabName() const;
	bool ContainsPrefab() const;

	bool GetActive() const;

	int32_t GetId() const;

};

template<class Comp>
inline Comp* IEntity::AddComponent() requires std::is_base_of_v<IComponent, Comp> {
	std::string name = typeid(Comp).name();
	if (name.find("class ") == 0) {
		name = name.substr(6);
	}
	return static_cast<Comp*>(AddComponent(name));
}

template<class Comp>
inline Comp* IEntity::GetComponent() const requires std::is_base_of_v<IComponent, Comp> {
	auto it = components_.find(GetComponentHash<Comp>());
	if (it != components_.end()) {
		return dynamic_cast<Comp*>(it->second);
	}
	return nullptr;
}

template<typename Comp>
inline void IEntity::RemoveComponent() requires std::is_base_of_v<IComponent, Comp> {
	std::string name = typeid(Comp).name();
	if (name.find("class ") == 0) {
		name = name.substr(6);
	}

	RemoveComponent(name);
}
