#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// engine
#include "../Component/ComponentArray/ComponentArray.h"
#include "../Component/Components/Interface/IComponent.h"
#include "../Component/Components/ComputeComponents/Transform/Transform.h"
#include "../Component/Components/ComputeComponents/Variables/Variables.h"
#include "../System/Interface/ECSISystem.h"

class Camera;
class Camera2D;


/// ===================================================
/// エンティティインターフェース
/// ===================================================
class IEntity {
	friend class EntityComponentSystem;
public:

	IEntity();

	void CommonInitialize();

	/// @brief 仮想デストラクタ
	virtual ~IEntity() {}

	/// @brief 初期化の純粋仮想関数
	virtual void Initialize() = 0;

	/// @brief 更新の純粋仮想関数
	virtual void Update() = 0;


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

	void UpdateTransform();

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	Transform* transform_;
	Variables* variables_;
	class EntityComponentSystem* pEntityComponentSystem_;

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



	const Vector3& GetLocalPosition() const;
	const Vector3& GetLocalRotate() const;
	const Vector3& GetLocalScale() const;

	Vector3 GetWorldPosition();
	Vector3 GetWorldRotate();
	Vector3 GetWorldScale();


	/// @brief transform の取得
	/// @return transform のポインタ
	Transform* GetTransform() const;

	const IEntity* GetParent() const;
	IEntity* GetParent();

	const std::vector<IEntity*>& GetChildren() const;
	IEntity* GetChild(size_t _index);

	const std::unordered_map<size_t, IComponent*>& GetComponents() const;


	/// @brief このエンティティの名前を設定する
	/// @return string型の名前
	const std::string& GetName() const;

};



/// ///////////////////////////////////////////////////
/// ECSの基盤クラス
/// ///////////////////////////////////////////////////
class EntityComponentSystem final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityComponentSystem(class DxManager* _pDxManager);
	~EntityComponentSystem();

	void Initialize();
	void Update();


	/// ----- entity ----- ///

	template<typename T>
	T* GenerateEntity() requires std::is_base_of_v<IEntity, T>;

	void RemoveEntity(IEntity* _entity);

	const std::vector<std::unique_ptr<IEntity>>& GetEntities() const { return entities_; }

	/// @brief 新しい camera を生成する
	/// @return cameraへのポインタ
	Camera* GenerateCamera();


	/// ----- component ----- ///

	template<typename Comp>
	Comp* AddComponent() requires std::is_base_of_v<IComponent, Comp>;

	template<typename Comp>
	Comp* GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

	template<typename Comp>
	void RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;



	/// ----- system ----- ///

	template<typename T, typename... Args>
	void AddSystem(Args... args) requires std::is_base_of_v<ECSISystem, T>;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager* pDxManager_ = nullptr;

	/// ----- entity ----- ///
	std::vector<std::unique_ptr<IEntity>> entities_;
	std::vector<Camera*> cameras_;
	Camera* mainCamera_ = nullptr;
	Camera* mainCamera2D_ = nullptr;
	Camera* debugCamera_ = nullptr;

	/// ----- component ----- ///
	std::unordered_map<size_t, std::unique_ptr<IComponentArray>> componentArrayMap_;

	/// ----- system ----- ///
	std::vector<std::unique_ptr<ECSISystem>> systemMap_;


public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	void SetMainCamera(Camera* _camera);
	void SetMainCamera(size_t _index);

	void SetMainCamera2D(Camera* _camera);
	void SetMainCamera2D(size_t _index);

	void SetDebugCamera(Camera* _camera);
	void SetDebugCamera(size_t _index);



	/// @brief entities の取得
	/// @return entities
	const std::vector<std::unique_ptr<IEntity>>& GetEntities();

	/// @brief cameras の取得
	/// @return cameras
	const std::vector<Camera*>& GetCameras();

	/// @brief main cameraの取得
	/// @return Cameraクラスへのポインタ
	const Camera* GetMainCamera() const;
	Camera* GetMainCamera();

	const Camera* GetMainCamera2D() const;
	Camera* GetMainCamera2D();

	const Camera* GetDebugCamera() const;
	Camera* GetDebugCamera();

};






/// ===================================================
/// inline methods
/// ===================================================

template<class T>
inline T* IEntity::AddComponent() requires std::is_base_of_v<IComponent, T> {

	size_t hash = typeid(T).hash_code();
	auto it = components_.find(hash);
	if (it != components_.end()) { ///< すでに同じコンポーネントが存在している場合
		return static_cast<T*>(it->second);
	}

	/// component の生成, 追加
	T* component = pEntityComponentSystem_->AddComponent<T>();
	component->SetOwner(this);
	components_[hash] = component;

	return component; ///< 生成した component のポインタを返す
}

template<class T>
inline T* IEntity::GetComponent() const requires std::is_base_of_v<IComponent, T> {
	auto it = components_.find(typeid(T).hash_code());
	if (it != components_.end()) {
		return dynamic_cast<T*>(it->second);
	}
	return nullptr;
}



/// ===================================================
/// inline methods
/// ===================================================

template<typename T>
inline T* EntityComponentSystem::GenerateEntity() requires std::is_base_of_v<IEntity, T> {
	std::unique_ptr<T> entity = std::make_unique<T>();
	entity->pEntityComponentSystem_ = this;
	entity->CommonInitialize();
	entity->Initialize();

	T* entityPtr = entity.get();
	entities_.push_back(std::move(entity));

	return entityPtr;
}

template<typename Comp>
inline Comp* EntityComponentSystem::AddComponent() requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = typeid(Comp).hash_code();

	if (componentArrayMap_.find(hash) == componentArrayMap_.end()) {
		componentArrayMap_[hash] = std::make_unique<ComponentArray<Comp>>();
	}

	{	/// componentを追加する

		ComponentArray<Comp>* componentArray = static_cast<ComponentArray<Comp>*>(componentArrayMap_[hash].get());

		if (componentArray->removedIndices_.size() > 0) { ///< 削除されたインデックスがある場合
			size_t index = componentArray->removedIndices_.back();
			componentArray->removedIndices_.pop_back();
			componentArray->usedIndices_.push_back(index);
			componentArray->components_[index] = Comp(); ///< 今までのデータを上書き

			return &componentArray->components_[index];
		}

		componentArray->components_.emplace_back();
		size_t index = componentArray->components_.size() - 1;
		componentArray->usedIndices_.push_back(index);

		return &componentArray->components_[index];
	}
}

template<typename Comp>
inline Comp* EntityComponentSystem::GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = typeid(Comp).hash_code();
	return &componentArrayMap_[hash]->components_[_index];
}

template<typename Comp>
inline void EntityComponentSystem::RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {
	size_t hash = typeid(Comp).hash_code();
	ComponentArray<Comp>* componentArray = static_cast<ComponentArray<Comp>*>(componentArrayMap_[hash].get());
	componentArray->usedIndices_.erase(std::remove(componentArray->usedIndices_.begin(), componentArray->usedIndices_.end(), _index), componentArray->usedIndices_.end());
	componentArray->removedIndices_.push_back(_index);
}

template<typename T, typename ...Args>
inline void EntityComponentSystem::AddSystem(Args ...args) requires std::is_base_of_v<ECSISystem, T> {

	systemMap_.push_back(std::make_unique<T>(args...));

}

