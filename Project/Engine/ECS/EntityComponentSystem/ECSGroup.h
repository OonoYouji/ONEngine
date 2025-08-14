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

	ECSGroup(class DxManager* _dxManager);
	~ECSGroup();

	void Initialize(const std::string& _groupName);
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
	void RemoveComponent(uint32_t _compId);
	void RemoveComponent(size_t _hash, uint32_t _compId);
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

	/// ----- parameters ----- ///
	std::string groupName_;

	/// ----- collections ----- ///
	std::unique_ptr<EntityCollection> entityCollection_;
	std::unique_ptr<ComponentCollection> componentCollection_;
	std::unique_ptr<SystemCollection> systemCollection_;

	/// ----- command ----- ///
	EntityDataInputCommand componentInputCommand_;


public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// ----- setter ----- ///

	void SetMainCamera(CameraComponent* _camera);
	void SetMainCamera2D(CameraComponent* _camera);


	/// ----- getter ----- ///

	const std::vector<std::unique_ptr<GameEntity>>& GetEntities() const;

	GameEntity* GetEntity(size_t _id) const;

	const CameraComponent* GetMainCamera() const;
	CameraComponent* GetMainCamera();
	const CameraComponent* GetMainCamera2D() const;
	CameraComponent* GetMainCamera2D();

	const std::string& GetGroupName() const;
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
inline void ECSGroup::RemoveComponent(uint32_t _compId) {
	componentCollection_->RemoveComponent<Comp>(_compId);
}

template<SystemType Sys, typename ...Args>
inline void ECSGroup::AddSystem(Args ..._args) {
	systemCollection_->AddSystem(std::make_unique<Sys>(_args...));
}
