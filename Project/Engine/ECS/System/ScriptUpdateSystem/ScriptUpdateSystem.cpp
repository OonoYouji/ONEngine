#include "ScriptUpdateSystem.h"

/// std
#include <list>

/// external
#include <mono/metadata/mono-gc.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"

ScriptUpdateSystem::ScriptUpdateSystem() {}
ScriptUpdateSystem::~ScriptUpdateSystem() {}

void ScriptUpdateSystem::Update([[maybe_unused]] EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) {

	{	/// debug monoのヒープの状態を出力
		size_t heapSize = mono_gc_get_heap_size();
		size_t usedSize = mono_gc_get_used_size();

		Console::Log("[debug] GC Heap Size : " + std::to_string(heapSize / 1024) + "KB");
		Console::Log("[debug] GC Used Size : " + std::to_string(usedSize / 1024) + "KB");
	}

	pScripts_.clear();
	for (auto& entity : _entities) {
		/// 親がいるなら無視(親から再帰的に処理するので
		if (entity->GetParent()) {
			continue;
		}

		RecursivePushBackScript(entity);
	}

	/// コンストラクタの呼びだし (内部でフラグで呼び出すか管理している)
	for (auto& scriptComp : pScripts_) {
		scriptComp->CallAwakeMethodAll();
	}

	/// 初期化関数の呼びだし (内部でフラグで呼び出すか管理している)
	for (auto& scriptComp : pScripts_) {
		scriptComp->CallInitMethodAll();
	}

	/// 更新関数の呼びだし
	for (auto& scriptComp : pScripts_) {
		scriptComp->CallUpdateMethodAll();
	}

	/// 行列の更新
	for (auto& script : pScripts_) {
		IEntity* entity = script->GetOwner();
		if (entity) {
			entity->UpdateTransform();
		}
	}
}

void ScriptUpdateSystem::RecursivePushBackScript(IEntity* _entity) {

	if (_entity) {

		/// エンティティが非アクティブなら無視、(子オブジェクトも更新しない)
		if (!_entity->GetActive()) {
			return;
		}

		Script* script = _entity->GetComponent<Script>();
		if (script && script->enable) {
			pScripts_.push_back(script);
		}

		/// 自身の子エンティティから再帰的に得る
		for (auto& child : _entity->GetChildren()) {
			RecursivePushBackScript(child);
		}
	}

}
