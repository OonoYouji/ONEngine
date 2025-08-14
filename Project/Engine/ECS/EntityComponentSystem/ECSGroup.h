#pragma once

/// engine
#include "../Entity/Collection/EntityCollection.h"
#include "../Component/Collection/ComponentCollection.h"
#include "../System/SystemCollection/SystemCollection.h"

#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"


template<typename T>
concept ComponentType = std::is_base_of_v<IComponent, T>;

template<typename T>
concept SystemType = std::is_base_of_v<ECSISystem, T>;

/// ///////////////////////////////////////////////////
/// ECSのコレクションのグループ
/// ///////////////////////////////////////////////////
class ECSGroup {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	ECSGroup();
	~ECSGroup();

	void Initialize();
	void Update();


	/// ----- entity ----- ///

	/// 生成
	GameEntity* GenerateEntity(bool _isRuntime);
	GameEntity* GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime = true);

	/// 削除
	void RemoveEntity(GameEntity* _entity, bool _deleteChildren = true);
	void RemoveEntityAll();

	/// 非破棄エンティティの追加と削除
	void AddDoNotDestroyEntity(GameEntity* _entity);
	void RemoveDoNotDestroyEntity(GameEntity* _entity);

	/// Id
	uint32_t GetEntityId(const std::string& _name);


	/// ----- component ----- ///

	/// 生成
	template<ComponentType Comp>
	Comp* AddComponent();
	IComponent* AddComponent(const std::string& _compName);

	/// 取得
	template<ComponentType Comp>
	Comp* GetComponent(size_t _entityId);
	template<ComponentType Comp>
	ComponentArray<Comp>* GetComponentArray();

	/// 削除
	template<ComponentType Comp>
	void RemoveComponent(size_t _entityId);
	void RemoveComponentAll(GameEntity* _entity);

	/// 読み込み
	void LoadComponent(GameEntity* _entity);


	/// ----- system ----- ///

	/// 追加
	template<SystemType Sys, typename... Args>
	void AddSystem(Args... _args);

	/// 更新
	void OutsideOfRuntimeUpdateSystems();
	void RuntimeUpdateSystems();


private:
	/// ===================================================
	/// public : objects
	/// ===================================================

	/// ----- collections ----- ///
	std::unique_ptr<EntityCollection> entityCollection_;
	std::unique_ptr<ComponentCollection> componentCollection_;
	std::unique_ptr<SystemCollection> systemCollection_;

	/// ----- command ----- ///
	EntityDataInputCommand componentInputCommand_;

};

/// ===================================================
/// inline methods
/// ===================================================

template<ComponentType Comp>
inline Comp* ECSGroup::AddComponent() {
	return componentCollection_->AddComponent<Comp>();
}

template<ComponentType Comp>
inline Comp* ECSGroup::GetComponent(size_t _entityId) {
	GameEntity* entity = entityCollection_->GetEntity(_entityId);
	if (entity) {
		return entity->GetComponent<Comp>();
	}

	return nullptr;
}

template<ComponentType Comp>
inline ComponentArray<Comp>* ECSGroup::GetComponentArray() {
	return componentCollection_->GetComponentArray<Comp>();
}

template<ComponentType Comp>
inline void ECSGroup::RemoveComponent(size_t _entityId) {
	GameEntity* entity = entityCollection_->GetEntity(_entityId);
	if (entity) {
		componentCollection_->RemoveComponent<Comp>(entity);
	}
}

template<SystemType Sys, typename ...Args>
inline void ECSGroup::AddSystem(Args ..._args) {
	systemCollection_->AddSystem(std::make_unique<Sys>(_args...));
}
