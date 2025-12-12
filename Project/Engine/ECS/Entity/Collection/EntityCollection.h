#pragma once

/// std
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <deque>

/// externals
#include <nlohmann/json.hpp>

#include "../GameEntity/GameEntity.h"
#include "../Prefab/EntityPrefab.h"

namespace ONEngine {

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

	EntityCollection(class ECSGroup* _ecsGroup, class DxManager* _dxm);
	~EntityCollection();

	/// 生成
	GameEntity* GenerateEntity(const Guid& _guid, bool _isRuntime = false);
	int32_t NewEntityID(bool _isRuntime);

	/// 取得
	uint32_t GetEntityId(const std::string& _name);
	GameEntity* GetEntity(size_t _entityId);
	GameEntity* GetEntityFromGuid(const Guid& _guid);

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

	/// prefabの内容をEntityに反映する
	void ApplyPrefabToEntity(GameEntity* _entity, const std::string& _prefabName);

private:


	/// @brief 再帰的にEntityを生成する
	GameEntity* GenerateEntityRecursive(const nlohmann::json& _json, GameEntity* _entity, bool _isRuntime);


private:
	/// =========================================
	/// private : objects
	/// =========================================

	class ECSGroup* pEcsGroup_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	/// entityのIDを管理するためのdeque
	IdContainer initEntityIDs_;
	IdContainer runtimeEntityIDs_;

	/// entityの本体を持つ配列
	std::vector<std::unique_ptr<GameEntity>> entities_;
	std::vector<GameEntity*> doNotDestroyEntities_;
	std::unordered_map<Guid, GameEntity*> guidEntityMap_;

	CameraComponent* mainCamera_ = nullptr;
	CameraComponent* mainCamera2D_ = nullptr;

	/// prefab
	std::unordered_map<std::string, std::unique_ptr<EntityPrefab>> prefabs_;

public:
	/// =========================================
	/// public : accessor
	/// =========================================

	void SetMainCamera(CameraComponent* _cameraComponent);
	void SetMainCamera2D(CameraComponent* _cameraComponent);

	CameraComponent* GetMainCamera();
	CameraComponent* GetMainCamera2D();

	const std::vector<std::unique_ptr<GameEntity>>& GetEntities() const;

};

} /// ONEngine
