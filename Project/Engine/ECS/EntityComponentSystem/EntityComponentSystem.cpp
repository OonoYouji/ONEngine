#include "EntityComponentSystem.h"

using namespace ONEngine;

/// std
#include <numbers>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"

/// ecs
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "AddECSSystemFunction.h"
#include "AddECSComponentFactoryFunction.h"

namespace {
	ECSGroup* gGameGroup = nullptr;
	ECSGroup* gDebugGroup = nullptr;
	EntityComponentSystem* gECS = nullptr;
}

void ONEngine::SetEntityComponentSystemPtr(ECSGroup* _gameGroup, ECSGroup* _debugGroup) {
	gGameGroup = _gameGroup;
	gDebugGroup = _debugGroup;
}

ECSGroup* ONEngine::GetEntityComponentSystemPtr() {
	return gGameGroup;
}

bool ONEngine::CheckParentEntityEnable(GameEntity* _entity) {
	/*
	* 確認事項
	* - 自身が有効なポインタであるか
	* - 親エンティティがあるか
	* - 親の親が有効か(再帰的に)
	*/

	if (!_entity) {
		return false;
	}

	GameEntity* parent = _entity->GetParent();
	if (parent) {
		if (!parent->active) {
			return false;
		}

		return CheckParentEntityEnable(parent);
	}

	return _entity->active;
}

bool ONEngine::CheckComponentEnable(IComponent* _component) {
	/*
	* 確認事項
	* - componentがnullptrでないこと
	* - componentが有効であること
	* - componentの所有者であるentityがnullptrでないこと
	* - entityが有効であること
	* - entityの親が有効であること
	*/

	if (!_component) {
		return false;
	}

	if (!_component->enable) {
		return false;
	}

	GameEntity* owner = _component->GetOwner();
	if (!owner) {
		return false;
	}

	if (!owner->active) {
		return false;
	}

	if (owner->GetParent()) {
		return CheckParentEntityEnable(owner);
	}


	return true;
}


EntityComponentSystem::EntityComponentSystem(DxManager* _pDxManager)
	: pDxManager_(_pDxManager) {

	gECS = this;
	//prefabCollection_ = std::make_unique<EntityPrefabCollection>();
}

EntityComponentSystem::~EntityComponentSystem() {}

void EntityComponentSystem::Initialize(AssetCollection* _assetCollection) {

	/// ポインタの確保
	pAssetCollection_ = _assetCollection;
	pDxDevice_ = pDxManager_->GetDxDevice();

	{	/// デバッグ用のECSGroupを作成、 追加するシステムが違うのでAddGroupは使わない
		currentGroupName_ = "Debug";
		std::unique_ptr<ECSGroup> debugGroup = std::make_unique<ECSGroup>(pDxManager_);
		debugGroup->Initialize(currentGroupName_);
		DebugECSGroupAddSystemFunction(debugGroup.get(), pDxManager_, pAssetCollection_);
		ecsGroups_[currentGroupName_] = std::move(debugGroup);
		debugGroup_ = ecsGroups_[currentGroupName_].get();
	}

}


void EntityComponentSystem::Update() {
	debugGroup_->RuntimeUpdateSystems();
	GetCurrentGroup()->RuntimeUpdateSystems();
}

void EntityComponentSystem::OutsideOfUpdate() {
	debugGroup_->OutsideOfRuntimeUpdateSystems();
	GetCurrentGroup()->OutsideOfRuntimeUpdateSystems();
}

void EntityComponentSystem::DebuggingUpdate() {
	SetEntityComponentSystemPtr(GetCurrentGroup(), debugGroup_);
}

ECSGroup* EntityComponentSystem::AddECSGroup(const std::string& _name) {
	/// すでに存在する場合は何もしない
	auto itr = ecsGroups_.find(_name);
	if (itr != ecsGroups_.end()) {
		Console::LogWarning("ECSGroup with name '" + _name + "' already exists.");
		return itr->second.get();
	}

	/// 新しいECSグループを作成
	std::unique_ptr<ECSGroup> newGroup = std::make_unique<ECSGroup>(pDxManager_);
	newGroup->Initialize(_name);
	GameECSGroupAddSystemFunction(newGroup.get(), pDxManager_, pAssetCollection_);
	ecsGroups_[_name] = std::move(newGroup);
	return ecsGroups_[_name].get();
}

ECSGroup* EntityComponentSystem::GetECSGroup(const std::string& _name) const {
	/// 指定された名前のECSグループが存在するかチェック
	auto it = ecsGroups_.find(_name);
	if (it != ecsGroups_.end()) {
		return it->second.get();
	}

	Console::LogWarning("ECSGroup with name '" + _name + "' not found.");
	return nullptr;
}

ECSGroup* EntityComponentSystem::GetCurrentGroup() const {
	/// 現在のグループを取得する
	if (ecsGroups_.empty()) {
		Console::LogWarning("No ECSGroup available.");
		return nullptr;
	}

	return ecsGroups_.at(currentGroupName_).get();
}

void EntityComponentSystem::MainCameraSetting() {
	auto Setting = [&](ECSGroup* _group)
		{
			/// 初期化時のmain cameraを設定する
			ComponentArray<CameraComponent>* cameraArray = _group->GetComponentArray<CameraComponent>();
			if (cameraArray) {
				for (auto& cameraComponent : cameraArray->GetUsedComponents()) {
					/// componentがnullptrでないことを確認、main cameraかどうかを確認
					if (cameraComponent && cameraComponent->GetIsMainCameraRequest()) {

						int type = cameraComponent->GetCameraType();
						if (type == static_cast<int>(CameraType::Type3D)) {
							_group->SetMainCamera(cameraComponent);
						} else if (type == static_cast<int>(CameraType::Type2D)) {
							_group->SetMainCamera2D(cameraComponent);
						}
					}
				}
			}
		};


	for (auto& group : ecsGroups_) {
		Setting(group.second.get());
	}
}

void EntityComponentSystem::SetCurrentGroupName(const std::string& _name) {
	currentGroupName_ = _name;
}

const std::string& EntityComponentSystem::GetCurrentGroupName() const {
	return currentGroupName_;
}

const std::unordered_map<std::string, std::unique_ptr<ECSGroup>>& EntityComponentSystem::GetECSGroups() const {
	return ecsGroups_;
}

void EntityComponentSystem::ReloadPrefab(const std::string& _prefabName) {
	for (auto& group : ecsGroups_) {
		auto entityCollection = group.second->GetEntityCollection();
		entityCollection->ReloadPrefab(_prefabName);
	}
}


/// ====================================================
/// internal methods
/// ====================================================

GameEntity* MONO_INTERNAL_METHOD::GetEntityById(int32_t _entityId, const std::string& _groupName) {
	ECSGroup* ecsGroup = gECS->GetECSGroup(_groupName);
	if (ecsGroup) {
		return ecsGroup->GetEntity(_entityId);
	}

	return nullptr;
}

uint64_t MONO_INTERNAL_METHOD::InternalAddComponent(int32_t _entityId, MonoString* _monoTypeName, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);

	std::string typeName = mono_string_to_utf8(_monoTypeName);
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	IComponent* component = entity->AddComponent(typeName);
	if (!component) {
		Console::Log("Failed to add component: " + typeName);
		return 0;
	}

	return reinterpret_cast<uint64_t>(component);
}

uint64_t MONO_INTERNAL_METHOD::InternalGetComponent(int32_t _entityId, MonoString* _monoTypeName, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);

	/// idからentityを取得
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	/// componentを取得
	std::string typeName = mono_string_to_utf8(_monoTypeName);
	IComponent* component = entity->GetComponent(typeName);

	return reinterpret_cast<uint64_t>(component);
}

const char* MONO_INTERNAL_METHOD::InternalGetName(int32_t _entityId, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return nullptr;
	}

	return entity->GetName().c_str();
}

void MONO_INTERNAL_METHOD::InternalSetName(int32_t _entityId, MonoString* _name, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);
	std::string name = mono_string_to_utf8(_name);
	GameEntity* entity = GetEntityById(_entityId, groupName);

	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	entity->SetName(name);
}

int32_t MONO_INTERNAL_METHOD::InternalGetChildId(int32_t _entityId, uint32_t _childIndex, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	const auto& children = entity->GetChildren();
	if (_childIndex >= children.size()) {
		Console::LogError("Child index out of range for entity ID: " + std::to_string(_entityId));
		return 0;
	}

	GameEntity* child = children[_childIndex];
	return child->GetId();
}

int32_t MONO_INTERNAL_METHOD::InternalGetChildrenCount(int32_t _entityId, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		return 0;
	}

	const auto& children = entity->GetChildren();
	return static_cast<int32_t>(children.size());
}

int32_t MONO_INTERNAL_METHOD::InternalGetParentId(int32_t _entityId, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	GameEntity* parent = entity->GetParent();
	if (parent) {
		return parent->GetId();
	}

	return 0;
}

void MONO_INTERNAL_METHOD::InternalSetParent(int32_t _entityId, int32_t _parentId, MonoString* _groupName) {
	const std::string groupName = mono_string_to_utf8(_groupName);
	GameEntity* entity = GetEntityById(_entityId, groupName);
	GameEntity* parent = GetEntityById(_parentId, groupName);

	/// nullptr チェック
	if (!entity || !parent) {
		Console::LogError("Entity or parent not found for IDs: " + std::to_string(_entityId) + ", " + std::to_string(_parentId));
		return;
	}

	/// idが同じ場合は何もしない
	if (entity->GetId() == parent->GetId()) {
		Console::LogError("Cannot set entity as its own parent: " + std::to_string(_entityId));
		return;
	}

	/// 既存の親を削除
	if (entity->GetParent()) {
		entity->RemoveParent();
	}

	/// 新しい親を設定
	entity->SetParent(parent);
}

void MONO_INTERNAL_METHOD::InternalAddScript(int32_t _entityId, MonoString* _scriptName, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);
	std::string scriptName = mono_string_to_utf8(_scriptName);
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	/// 追加されていなければスクリプトを追加
	Script* script = entity->AddComponent<Script>();
	if (!script->Contains(scriptName)) {
		script->AddScript(scriptName);
		Script::ScriptData* data = script->GetScriptData(scriptName);
		if (data) {
			data->isAdded = true;
		}
	}
}

bool MONO_INTERNAL_METHOD::InternalGetScript(int32_t _entityId, MonoString* _scriptName, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);
	GameEntity* entity = GetEntityById(_entityId, groupName);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return false;
	}

	Script* script = entity->GetComponent<Script>();
	if (!script) {
		Console::LogError("Script component not found for Entity ID: " + std::to_string(_entityId));
		return false;
	}

	char* cstr = mono_string_to_utf8(_scriptName);
	std::string scriptName(cstr);
	mono_free(cstr);

	if (script->Contains(scriptName)) {
		Console::Log(std::format("Script {} found for Entity ID: {}", scriptName, _entityId));
		return true;
	}

	return false;
}

void MONO_INTERNAL_METHOD::InternalCreateEntity(int32_t* _entityId, MonoString* _prefabName, MonoString* _groupName) {
	std::string groupName = mono_string_to_utf8(_groupName);
	ECSGroup* group = gECS->GetECSGroup(groupName);

	/// prefabを検索
	std::string prefabName = mono_string_to_utf8(_prefabName);
	GameEntity* entity = group->GenerateEntityFromPrefab(prefabName + ".prefab");
	if (!entity) {
		entity = group->GenerateEntity(GenerateGuid(), true);
		if (!entity) {
			return;
		}
	}

	/// EntityのScriptのAddedをTrueにする
	if (Script* script = entity->GetComponent<Script>()) {
		script->SetIsAdded(true);
	}

	if (_entityId) {
		*_entityId = entity->GetId();
	}
}

void MONO_INTERNAL_METHOD::InternalDestroyEntity(MonoString* _ecsGroupName, int32_t _entityId) {
	char* cstr = mono_string_to_utf8(_ecsGroupName);
	ECSGroup* group = gECS->GetECSGroup(cstr);

	if (!group) {
		Console::LogError("ECSGroup not found: " + std::string(cstr));
		return;
	}

	GameEntity* entity = group->GetEntity(_entityId);
	group->RemoveEntity(entity);


	mono_free(cstr);
}

bool MONO_INTERNAL_METHOD::InternalGetEnable(int32_t _entityId, MonoString* _ecsGroupName) {
	/// ECSGroupの取得
	std::string groupName = mono_string_to_utf8(_ecsGroupName);
	ECSGroup* group = gECS->GetECSGroup(groupName);

	/// Entityの取得
	GameEntity* entity = group->GetEntity(_entityId);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return false;
	}

	return entity->active;
}

void MONO_INTERNAL_METHOD::InternalSetEnable(int32_t _entityId, bool _enable, MonoString* _ecsGroupName) {
	/// ECSGroupの取得
	std::string groupName = mono_string_to_utf8(_ecsGroupName);
	ECSGroup* group = gECS->GetECSGroup(groupName);

	/// Entityの取得
	GameEntity* entity = group->GetEntity(_entityId);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	/// Entityの有効化・無効化
	entity->active = _enable;
}
