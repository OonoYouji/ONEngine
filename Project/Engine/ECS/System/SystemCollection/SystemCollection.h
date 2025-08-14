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

	/// @brief 新規systemを追加する
	void AddSystem(std::unique_ptr<ECSISystem> _system);

	/// @brief runtime外の更新処理を行う (runtime中でも処理される)
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs);

	/// @brief runtime中の更新処理を行う
	void RuntimeUpdate(class ECSGroup* _ecs);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<ECSISystem>> systems_;

};

