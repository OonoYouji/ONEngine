#pragma once

/// std
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <deque>

#include "../GameEntity/GameEntity.h"
#include "../Prefab/EntityPrefab.h"

class CameraComponent;

/// ///////////////////////////////////////////////////
/// Entityのコレクションクラス
/// ///////////////////////////////////////////////////
class EntityCollection final {
private:
	/// =========================================
	/// private : sub classes
	/// =========================================

	/// @brief EntityIdの管理用コンテナ
	struct IdContainer {
		std::deque<int32_t> usedIds;    ///< 使用中のID
		std::deque<int32_t> removedIds; ///< 削除されたID
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	EntityCollection(class ECSGroup* _ecsGroup, class DxManager* _dxManager);
	~EntityCollection();

	/// 生成
	GameEntity* GenerateEntity(bool _isRuntime = false);
	int32_t NewEntityID(bool _isRuntime);

	/// 取得
	uint32_t GetEntityId(const std::string& _name);
	GameEntity* GetEntity(size_t _entityId);

	/// 削除
	void RemoveEntity(GameEntity* _entity, bool _deleteChildren = true);
	void RemoveEntityId(int32_t _id);
	void RemoveEntityAll();

	/// 非破棄エンティティの追加と削除
	void AddDoNotDestroyEntity(GameEntity* _entity);
	void RemoveDoNotDestroyEntity(GameEntity* _entity);




	/* ----- prefab ----- */

	void LoadPrefabAll();
	void ReloadPrefab(const std::string& _prefabName);

	GameEntity* GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime = true);
	EntityPrefab* GetPrefab(const std::string& _fileName);

private:
	/// =========================================
	/// private : objects
	/// =========================================

	class ECSGroup* pECSGroup_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	/// entityのIDを管理するためのdeque
	IdContainer initEntityIDs_;
	IdContainer runtimeEntityIDs_;

	/// entityの本体を持つ配列
	std::vector<std::unique_ptr<GameEntity>> entities_;
	std::vector<GameEntity*> doNotDestroyEntities_;

	CameraComponent* mainCamera_ = nullptr;
	CameraComponent* mainCamera2D_ = nullptr;

	/// prefab
	std::unordered_map<std::string, std::unique_ptr<EntityPrefab>> prefabs_;

public:

	void SetMainCamera(CameraComponent* _CameraComponent);
	void SetMainCamera2D(CameraComponent* _CameraComponent);

	CameraComponent* GetMainCamera();
	CameraComponent* GetMainCamera2D();

	const std::vector<std::unique_ptr<GameEntity>>& GetEntities() const;

};
