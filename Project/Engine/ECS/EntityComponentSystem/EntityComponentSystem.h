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

	void Initialize(class AssetCollection* _graphicsResourceCollection);
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


	/// ----- prefab ----- ///

	void ReloadPrefab(const std::string& _prefabName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other objects ----- ///
	class AssetCollection* pGraphicsResourceCollection_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	/// ----- groups ----- ///
	std::unordered_map<std::string, std::unique_ptr<ECSGroup>> ecsGroups_;
	ECSGroup* debugGroup_ = nullptr;
	std::string currentGroupName_;

	///// ----- prefab ----- ///
	//std::unique_ptr<EntityPrefabCollection> prefabCollection_;

};


/// =============================================
/// monoを使ったC#スクリプトエンジンのコンポーネント
/// =============================================

namespace MONO_INTERNAL_METHOD {

	/// エンティティのidからEntityを取得
	GameEntity* GetEntityById(int32_t _entityId, const std::string& _groupName);

	uint64_t InternalAddComponent(int32_t _entityId, MonoString* _monoTypeName, MonoString* _groupName);
	uint64_t InternalGetComponent(int32_t _entityId, MonoString* _monoTypeName, MonoString* _groupName);
	const char* InternalGetName(int32_t _entityId, MonoString* _groupName);
	void InternalSetName(int32_t _entityId, MonoString* _name, MonoString* _groupName);
	int32_t InternalGetChildId(int32_t _entityId, uint32_t _childIndex, MonoString* _groupName);
	int32_t InternalGetParentId(int32_t _entityId, MonoString* _groupName);
	void InternalSetParent(int32_t _entityId, int32_t _parentId, MonoString* _groupName);
	void InternalAddScript(int32_t _entityId, MonoString* _scriptName, MonoString* _groupName);
	bool InternalGetScript(int32_t _entityId, MonoString* _scriptName, MonoString* _groupName);

	void InternalCreateEntity(int32_t* _entityId, MonoString* _prefabName, MonoString* _groupName);

	void InternalDestroyEntity(MonoString* _ecsGroupName, int32_t _entityId);

} // namespace MONO_INTERNAL_METHOD
