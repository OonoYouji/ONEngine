#pragma once

/// std
#include <unordered_map>
#include <memory>
#include <string>

/// externals
#include <jit/jit.h>

/// engine
#include "ECSGroup.h"
#include "../Entity/Collection/EntityCollection.h"
#include "../Entity/Prefab/EntityPrefabCollection.h"
#include "../Component/Collection/ComponentCollection.h"
#include "../System/SystemCollection/SystemCollection.h"

#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"

class CameraComponent;

void SetEntityComponentSystemPtr(ECSGroup* _ecs);
ECSGroup* GetEntityComponentSystemPtr();


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
	void OutsideOfUpdate();

	void DebuggingUpdate();

	/// ----- group  ----- ///

	void MainCameraSetting();


	/// ----- prefab ----- ///

	//void ReloadPrefab(const std::string& _prefabName);
	//GameEntity* GetPrefabEntity() const;
	//GameEntity* GeneratePrefabEntity(const std::string& _name);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other objects ----- ///
	class GraphicsResourceCollection* pGraphicsResourceCollection_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	/// ----- groups ----- ///
	std::unique_ptr<ECSGroup> gameGroup_;
	std::unique_ptr<ECSGroup> debugGroup_;

	/// ----- prefab ----- ///
	std::unique_ptr<EntityPrefabCollection> prefabCollection_;

public:
	/// ===================================================
	/// public : accessor
	/// ====================================================

	ECSGroup* GetGameECSGroup() const;
	ECSGroup* GetDebugECSGroup() const;

};


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