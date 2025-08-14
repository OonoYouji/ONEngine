#include "EntityComponentSystem.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"

/// ecs
#include "../Component/Component.h"
#include "AddECSSystemFunction.h"
#include "AddECSComponentFactoryFunction.h"

namespace {
	ECSGroup* gGameGroup = nullptr;
	ECSGroup* gDebugGroup = nullptr;
}

void SetEntityComponentSystemPtr(ECSGroup* _gameGroup, ECSGroup* _debugGroup) {
	gGameGroup = _gameGroup;
	gDebugGroup = _debugGroup;
}

ECSGroup* GetEntityComponentSystemPtr() {
	return gGameGroup;
}


EntityComponentSystem::EntityComponentSystem(DxManager* _pDxManager)
	: pDxManager_(_pDxManager) {

	prefabCollection_ = std::make_unique<EntityPrefabCollection>();
}

EntityComponentSystem::~EntityComponentSystem() {}

void EntityComponentSystem::Initialize(GraphicsResourceCollection* _graphicsResourceCollection) {

	/// ポインタの確保
	pGraphicsResourceCollection_ = _graphicsResourceCollection;
	pDxDevice_ = pDxManager_->GetDxDevice();

	{	/// デバッグ用のECSGroupを作成、 追加するシステムが違うのでAddGroupは使わない
		currentGroupName_ = "Debug";
		std::unique_ptr<ECSGroup> debugGroup = std::make_unique<ECSGroup>(pDxManager_);
		debugGroup->Initialize(currentGroupName_);
		DebugECSGroupAddSystemFunction(debugGroup.get(), pDxManager_, pGraphicsResourceCollection_);
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

void EntityComponentSystem::DebuggingUpdate() {}

void EntityComponentSystem::AddECSGroup(const std::string& _name) {
	/// すでに存在する場合は何もしない
	if (ecsGroups_.find(_name) != ecsGroups_.end()) {
		Console::LogWarning("ECSGroup with name '" + _name + "' already exists.");
		return;
	}

	/// 新しいECSグループを作成
	std::unique_ptr<ECSGroup> newGroup = std::make_unique<ECSGroup>(pDxManager_);
	newGroup->Initialize(_name);
	GameECSGroupAddSystemFunction(newGroup.get(), pDxManager_, pGraphicsResourceCollection_);
	ecsGroups_[_name] = std::move(newGroup);
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
					if (cameraComponent && cameraComponent->GetIsMainCamera()) {

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


/// ====================================================
/// internal methods
/// ====================================================

GameEntity* GetEntityById(int32_t _entityId) {
	GameEntity* entity = gGameGroup->GetEntity(_entityId);
	if (!entity) {
		entity = gDebugGroup->GetEntity(_entityId);
		/// prefab entityじゃないかチェック
		//if (gECS->GetPrefabEntity() && gECS->GetPrefabEntity()->GetId() == _entityId) {
		//	entity = gECS->GetPrefabEntity();
		//}

		/// それでも違ったらエラーを出力
		if (!entity) {
			Console::Log("Entity not found for ID: " + std::to_string(_entityId));
			return nullptr;
		}
	}

	return entity;
}

uint64_t InternalAddComponent(int32_t _entityId, MonoString* _monoTypeName) {
	std::string typeName = mono_string_to_utf8(_monoTypeName);
	GameEntity* entity = GetEntityById(_entityId);
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

uint64_t InternalGetComponent(int32_t _entityId, MonoString* _monoTypeName) {
	/// idからentityを取得
	GameEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	/// componentを取得
	std::string typeName = mono_string_to_utf8(_monoTypeName);
	IComponent* component = entity->GetComponent(typeName);

	return reinterpret_cast<uint64_t>(component);
}

const char* InternalGetName(int32_t _entityId) {
	GameEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return nullptr;
	}

	return entity->GetName().c_str();
}

void InternalSetName(int32_t _entityId, MonoString* _name) {
	std::string name = mono_string_to_utf8(_name);
	GameEntity* entity = GetEntityById(_entityId);

	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	entity->SetName(name);
}

int32_t InternalGetChildId(int32_t _entityId, uint32_t _childIndex) {
	GameEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	const auto& children = entity->GetChildren();
	if (_childIndex >= children.size()) {
		Console::Log("Child index out of range for entity ID: " + std::to_string(_entityId));
		return 0;
	}

	GameEntity* child = children[_childIndex];
	return child->GetId();
}

int32_t InternalGetParentId(int32_t _entityId) {
	GameEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return 0;
	}

	GameEntity* parent = entity->GetParent();
	if (parent) {
		return parent->GetId();
	}

	return 0;
}

void InternalSetParent(int32_t _entityId, int32_t _parentId) {
	GameEntity* entity = GetEntityById(_entityId);
	GameEntity* parent = GetEntityById(_parentId);

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

void InternalAddScript(int32_t _entityId, MonoString* _scriptName) {
	std::string scriptName = mono_string_to_utf8(_scriptName);
	GameEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::Log("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	/// 追加されていなければスクリプトを追加
	Script* script = entity->AddComponent<Script>();
	if (!script->Contains(scriptName)) {
		script->AddScript(scriptName);
	}
}


bool InternalGetScript(int32_t _entityId, MonoString* _scriptName) {
	GameEntity* entity = GetEntityById(_entityId);
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

bool InternalContainsEntity(int32_t _entityId) {
	GameEntity* entity = GetEntityById(_entityId);
	if (entity) {
		return true;
	}

	return false;
}

int32_t InternalGetEntityId(MonoString* _name) {
	std::string name = mono_string_to_utf8(_name);
	return gGameGroup->GetEntityId(name);
}

int32_t InternalCreateEntity(MonoString* _name) {
	/// prefabを検索
	std::string name = mono_string_to_utf8(_name);
	GameEntity* entity = gGameGroup->GenerateEntityFromPrefab(name + ".prefab");
	if (!entity) {
		entity = gGameGroup->GenerateEntity(true);
		if (!entity) {
			return 0;
		}
	}

	Script* script = entity->GetComponent<Script>();
	if (script) {
		/// スクリプトの初期化を行う
		script->CallAwakeMethodAll();
		script->CallInitMethodAll();
	}

	return entity->GetId();
}

bool InternalContainsPrefabEntity(int32_t _entityId) {
	//GameEntity* entity = gECS->GetPrefabEntity();
	//if (entity) {
	//	if (entity->GetId() == _entityId) {
	//		return true;
	//	}
	//}

	return false;
}

void InternalDestroyEntity(int32_t _entityId) {
	GameEntity* entity = GetEntityById(_entityId);
	if (!entity) {
		Console::LogError("Entity not found for ID: " + std::to_string(_entityId));
		return;
	}

	gGameGroup->RemoveEntity(entity, true);
	Console::Log("Entity destroyed: " + entity->GetName() + " (ID: " + std::to_string(_entityId) + ")");
}

