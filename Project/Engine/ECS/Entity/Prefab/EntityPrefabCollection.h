#pragma once

/// std
#include <string>

/// engine
#include "EntityPrefab.h"
#include "../GameEntity/GameEntity.h"

/// ///////////////////////////////////////////////////
/// エンティティのプレハブを管理するクラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

class EntityPrefabCollection final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityPrefabCollection() = default;
	~EntityPrefabCollection() = default;

	/// 読み込み
	void LoadPrefabAll();
	void ReloadPrefab(const std::string& _prefabName);
	
	/// 取得
	EntityPrefab* GetPrefab(const std::string& _prefabName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map<std::string, std::unique_ptr<EntityPrefab>> prefabs_;

};


} /// ONEngine
