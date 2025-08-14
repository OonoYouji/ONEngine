#pragma once

/// std
#include <unordered_map>
#include <memory>

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// ECSのシステムを管理するクラス
/// ///////////////////////////////////////////////////
class SystemCollection final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SystemCollection() = default;
	~SystemCollection() = default;

	void Update(class EntityComponentSystem* _ecs);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================


	std::vector<std::unique_ptr<ECSISystem>> systems_;

};

