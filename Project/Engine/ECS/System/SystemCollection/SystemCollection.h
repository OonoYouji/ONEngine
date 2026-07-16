#pragma once

/// std
#include <unordered_map>
#include <memory>

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// ECSのシステムを管理するクラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class SystemCollection
 * @brief 登録されたすべての ECS システム（ECSISystem）を保持し、エディタ編集時/ランタイム実行時それぞれの毎フレーム一括更新呼び出しを管理するクラス
 */
class SystemCollection final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	SystemCollection() = default;

	/**
	 * @brief デストラクタ
	 */
	~SystemCollection() = default;

	/**
	 * @brief 管理下へ新しい ECS システムを追加し登録します。
	 * @param _system 登録するシステムの一意ポインタ
	 */
	void AddSystem(std::unique_ptr<ECSISystem> _system);

	/**
	 * @brief エディタ停止中（非ランタイム実行時）に、登録された全システムの OutsideOfRuntimeUpdate を順に呼び出します。
	 * @param _ecs 対象となるECSグループへのポインタ
	 */
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs);

	/**
	 * @brief ランタイム実行中に、登録された全システムの RuntimeUpdate を順に呼び出します。
	 * @param _ecs 対象となるECSグループへのポインタ
	 */
	void RuntimeUpdate(class ECSGroup* _ecs);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<ECSISystem>> systems_;

};


} /// ONEngine
