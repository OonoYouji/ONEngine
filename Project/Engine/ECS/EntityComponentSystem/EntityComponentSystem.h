#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// engine
#include "../Entity/Collection/EntityCollection.h"
#include "../Component/Collection/ComponentCollection.h"
#include "../System/Interface/ECSISystem.h"

#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"

class Camera;
class Camera2D;

void SetEntityComponentSystemPtr(EntityComponentSystem* _ecs);
EntityComponentSystem* GetEntityComponentSystemPtr();


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

	IEntity* GenerateEntity(const std::string& _name);

	void RemoveEntity(IEntity* _entity, bool _deleteChildren = true);

	/// @brief 新しい camera を生成する
	/// @return cameraへのポインタ
	Camera* GenerateCamera();

	template<typename T>
	T* GenerateCamera() requires std::is_base_of_v<Camera, T>;

	void RemoveEntityAll();

	template <typename T>
	T* FindEntity() requires std::is_base_of_v<IEntity, T>;

	void AddDoNotDestroyEntity(IEntity* _entity);
	void RemoveDoNotDestroyEntity(IEntity* _entity);

	void SetFactoryRegisterFunc(std::function<void(EntityFactory*)> _func);


	/// ----- component ----- ///

	template<typename Comp>
	Comp* AddComponent() requires std::is_base_of_v<IComponent, Comp>;

	IComponent* AddComponent(const std::string& _name);

	template<typename Comp>
	Comp* GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

	template<typename Comp>
	void RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

	void RemoveComponent(size_t _hash, size_t _id);

	void LoadComponent(IEntity* _entity);

	void RemoveComponentAll(IEntity* _entity);


	template<typename Comp>
	ComponentArray<Comp>* GetComponentArray() requires std::is_base_of_v<IComponent, Comp>;



	/// ----- system ----- ///

	template<typename T, typename... Args>
	void AddSystem(Args... args) requires std::is_base_of_v<ECSISystem, T>;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	std::unique_ptr<EntityCollection> entityCollection_;
	std::unique_ptr<ComponentCollection> componentCollection_;

	/// ----- system ----- ///
	std::vector<std::unique_ptr<ECSISystem>> systemMap_;


	/// ----- command ----- ///
	EntityDataInputCommand componentInputCommand_;

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

	IEntity* GetEntity(size_t _index);

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

template<typename T>
inline T* EntityComponentSystem::GenerateEntity() requires std::is_base_of_v<IEntity, T> {
	return entityCollection_->GenerateEntity<T>();
}

template<typename T>
inline T* EntityComponentSystem::GenerateCamera() requires std::is_base_of_v<Camera, T> {
	return entityCollection_->GenerateCamera<T>();
}

template<typename T>
inline T* EntityComponentSystem::FindEntity() requires std::is_base_of_v<IEntity, T> {
	return entityCollection_->FindEntity<T>();
}

template<typename Comp>
inline Comp* EntityComponentSystem::AddComponent() requires std::is_base_of_v<IComponent, Comp> {
	return componentCollection_->AddComponent<Comp>();
}

template<typename Comp>
inline Comp* EntityComponentSystem::GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {

	IEntity* entity = entityCollection_->GetEntities()[_index].get();
	if (entity) {
		return entity->GetComponent<Comp>();
	}

	return nullptr;
}

template<typename Comp>
inline void EntityComponentSystem::RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {
	return componentCollection_->RemoveComponent<Comp>(_index);
}

template<typename Comp>
inline ComponentArray<Comp>* EntityComponentSystem::GetComponentArray() requires std::is_base_of_v<IComponent, Comp> {
	return componentCollection_->GetComponentArray<Comp>();
}

template<typename T, typename ...Args>
inline void EntityComponentSystem::AddSystem(Args ...args) requires std::is_base_of_v<ECSISystem, T> {
	systemMap_.push_back(std::make_unique<T>(args...));
}

