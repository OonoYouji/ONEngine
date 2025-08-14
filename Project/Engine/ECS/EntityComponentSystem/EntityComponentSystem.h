#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// externals
#include <jit/jit.h>

/// engine
#include "../Entity/Collection/EntityCollection.h"
#include "../Component/Collection/ComponentCollection.h"
#include "../System/Interface/ECSISystem.h"

#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"

class CameraComponent;

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

	void Initialize(class GraphicsResourceCollection* _graphicsResourceCollection);
	void Update();

	void DebuggingUpdate();



	/// ----- entity ----- ///

	GameEntity* GenerateEntity(bool _isRuntime);

	GameEntity* GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime = true);

	void RemoveEntity(GameEntity* _entity, bool _deleteChildren = true);

	void RemoveEntityAll();

	void AddDoNotDestroyEntity(GameEntity* _entity);
	void RemoveDoNotDestroyEntity(GameEntity* _entity);

	uint32_t GetEntityId(const std::string& _name);

	/// ----- component ----- ///

	template<typename Comp>
	Comp* AddComponent() requires std::is_base_of_v<IComponent, Comp>;

	IComponent* AddComponent(const std::string& _name);

	template<typename Comp>
	Comp* GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

	template<typename Comp>
	void RemoveComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp>;

	void RemoveComponent(size_t _hash, size_t _id);

	void LoadComponent(GameEntity* _entity);

	void RemoveComponentAll(GameEntity* _entity);


	template<typename Comp>
	ComponentArray<Comp>* GetComponentArray() requires std::is_base_of_v<IComponent, Comp>;



	/// ----- system ----- ///

	template<typename T, typename... Args>
	void AddSystem(Args... args) requires std::is_base_of_v<ECSISystem, T>;

	void RuntimeUpdateSystems();
	void OutsideOfRuntimeUpdateSystems();


	/// ----- prefab ----- ///

	void ReloadPrefab(const std::string& _prefabName);
	GameEntity* GetPrefabEntity() const;
	GameEntity* GeneratePrefabEntity(const std::string& _name);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/* ----- other classes ----- */
	class GraphicsResourceCollection* pGraphicsResourceCollection_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	std::unique_ptr<EntityCollection> entityCollection_;
	std::unique_ptr<ComponentCollection> componentCollection_;

	/// ----- system ----- ///
	std::vector<std::unique_ptr<ECSISystem>> systems_;


	/// ----- command ----- ///
	EntityDataInputCommand componentInputCommand_;


	/// ----- editor ----- ///
	std::unique_ptr<GameEntity> debugCamera_;
	std::unique_ptr<GameEntity> prefabEntity_;


public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	void SetMainCamera(CameraComponent* _camera);
	void SetMainCamera2D(CameraComponent* _camera);

	/// @brief entities の取得
	/// @return entities
	const std::vector<std::unique_ptr<GameEntity>>& GetEntities();

	GameEntity* GetEntity(size_t _index);

	/// @brief main cameraの取得
	/// @return Cameraクラスへのポインタ
	const CameraComponent* GetMainCamera() const;
	CameraComponent* GetMainCamera();

	const CameraComponent* GetMainCamera2D() const;
	CameraComponent* GetMainCamera2D();

	const CameraComponent* GetDebugCamera() const;
	CameraComponent* GetDebugCamera();

};



/// ===================================================
/// inline methods
/// ===================================================

template<typename Comp>
inline Comp* EntityComponentSystem::AddComponent() requires std::is_base_of_v<IComponent, Comp> {
	return componentCollection_->AddComponent<Comp>();
}

template<typename Comp>
inline Comp* EntityComponentSystem::GetComponent(size_t _index) requires std::is_base_of_v<IComponent, Comp> {

	GameEntity* entity = entityCollection_->GetEntities()[_index].get();
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
	systems_.push_back(std::make_unique<T>(args...));
}


/// =============================================
/// monoを使ったC#スクリプトエンジンのコンポーネント
/// =============================================

GameEntity* GetEntityById(int32_t _entityId);

uint64_t InternalAddComponent(int32_t _entityId, MonoString* _monoTypeName);
uint64_t InternalGetComponent(int32_t _entityId, MonoString* _monoTypeName);
const char* InternalGetName(int32_t _entityId);
void InternalSetName(int32_t _entityId, MonoString* _name);
int32_t InternalGetChildId(int32_t _entityId, uint32_t _childIndex);
int32_t InternalGetParentId(int32_t _entityId);
void InternalSetParent(int32_t _entityId, int32_t _parentId);
void InternalAddScript(int32_t _entityId, MonoString* _scriptName);
bool InternalGetScript(int32_t _entityId, MonoString* _scriptName);

bool InternalContainsEntity(int32_t _entityId);
int32_t InternalGetEntityId(MonoString* _name);
int32_t InternalCreateEntity(MonoString* _name);
bool InternalContainsPrefabEntity(int32_t _entityId);
void InternalDestroyEntity(int32_t _entityId);