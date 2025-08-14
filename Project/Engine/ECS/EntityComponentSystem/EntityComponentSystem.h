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

void SetEntityComponentSystemPtr(ECSGroup* _gameGroup, ECSGroup* _debugGroup);
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

	/// 追加
	ECSGroup* AddECSGroup(const std::string& _name);

	/// 取得
	ECSGroup* GetECSGroup(const std::string& _name) const;
	ECSGroup* GetCurrentGroup() const;

	/// main cameraの設定
	void MainCameraSetting();

	/// 現在のグループ
	void SetCurrentGroupName(const std::string& _name);
	const std::string& GetCurrentGroupName() const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other objects ----- ///
	class GraphicsResourceCollection* pGraphicsResourceCollection_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	/// ----- groups ----- ///
	std::unordered_map<std::string, std::unique_ptr<ECSGroup>> ecsGroups_;
	ECSGroup* debugGroup_ = nullptr;
	std::string currentGroupName_;

	/// ----- prefab ----- ///
	std::unique_ptr<EntityPrefabCollection> prefabCollection_;

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