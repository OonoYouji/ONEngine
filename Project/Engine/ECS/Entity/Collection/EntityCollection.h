#pragma once

/// std
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <deque>

#include "../Factory/EntityFactory.h"
#include "../Prefab/EntityPrefab.h"

class CameraComponent;


class EntityCollection final {

	/// @brief EntityIdの管理用コンテナ
	struct IdContainer {
		std::deque<int32_t> usedIds;    ///< 使用中のID
		std::deque<int32_t> removedIds; ///< 削除されたID
	};


public:

	EntityCollection(class EntityComponentSystem* _ecs, class DxManager* _dxManager);
	~EntityCollection();

	IEntity* GenerateEntity(const std::string& _name, bool _isInit, bool _isRuntime = false);

	void RemoveEntity(IEntity* _entity, bool _deleteChildren = true);
	void RemoveEntityId(int32_t _id);

	void RemoveEntityAll();

	/// @brief 全エンティティを更新
	void UpdateEntities();

	/// @brief 引数のエンティティの更新(子も同時に
	/// @param _entity 更新したいエンティティ
	void UpdateEntity(IEntity* _entity);

	void AddDoNotDestroyEntity(IEntity* _entity);
	void RemoveDoNotDestroyEntity(IEntity* _entity);

	void SetFactoryRegisterFunc(std::function<void(EntityFactory*)> _func);

	int32_t NewEntityID(bool _isRuntime);

	uint32_t GetEntityId(const std::string& _name);

	/* ----- prefab ----- */

	void LoadPrefabAll();
	void ReloadPrefab(const std::string& _prefabName);

	IEntity* GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime = true);
	EntityPrefab* GetPrefab(const std::string& _fileName);

private:

	class EntityComponentSystem* pECS_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	std::unique_ptr<EntityFactory> factory_;

	/// entityのIDを管理するためのdeque
	IdContainer initEntityIDs_;
	IdContainer runtimeEntityIDs_;

	/// entityの本体を持つ配列
	std::vector<std::unique_ptr<IEntity>> entities_;
	std::vector<IEntity*> doNotDestroyEntities_;

	CameraComponent* debugCamera_ = nullptr;
	CameraComponent* mainCamera_ = nullptr;
	CameraComponent* mainCamera2D_ = nullptr;

	std::function<void(EntityFactory*)> factoryRegisterFunc_;


	/// prefab
	std::unordered_map<std::string, std::unique_ptr<EntityPrefab>> prefabs_;

public:

	void SetMainCamera(CameraComponent* _CameraComponent);
	void SetMainCamera2D(CameraComponent* _CameraComponent);
	void SetDebugCamera(CameraComponent* _CameraComponent);

	CameraComponent* GetMainCamera();
	CameraComponent* GetMainCamera2D();


	const std::vector<std::unique_ptr<IEntity>>& GetEntities() const;

	const CameraComponent* GetDebugCamera() const;
	CameraComponent* GetDebugCamera();

	EntityFactory* GetFactory();

};
