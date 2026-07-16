#include "SystemCollection.h"

using namespace ONEngine;

#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"

/**
 * @brief 管理下へ新しい ECS システムを追加し登録します。
 */
void SystemCollection::AddSystem(std::unique_ptr<ECSISystem> _system) {
	if (_system) {
		systems_.emplace_back(std::move(_system));
	}
}

/**
 * @brief エディタ停止中（非ランタイム実行時）に、登録された全システムの OutsideOfRuntimeUpdate を順に呼び出します。
 */
void SystemCollection::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	for (auto& system : systems_) {
		if (system) {
			system->OutsideOfRuntimeUpdate(_ecs);
		}
	}
}

/**
 * @brief ランタイム実行中に、登録された全システムの RuntimeUpdate を順に呼び出します。
 */
void SystemCollection::RuntimeUpdate(ECSGroup* _ecs) {
	for (auto& system : systems_) {
		if (system) {
			system->RuntimeUpdate(_ecs);
		}
	}
}
