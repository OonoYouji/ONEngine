#pragma once

/// std
#include <vector>

/// //////////////////////////////////////////////////
/// ECSのSystemのinterfaceクラス
/// //////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ECSISystem
 * @brief すべての ECS (Entity Component System) システムの基底インターフェースクラス
 */
class ECSISystem {
public:

	/**
	 * @brief デストラクタ
	 */
	virtual ~ECSISystem() {}

	/**
	 * @brief エディタ停止中（非ランタイム実行時）にシステム固有の更新処理を行うメソッド（仮想関数、必要に応じて派生クラスでオーバーライド）
	 * @param _ecs 対象となるECSグループへのポインタ
	 */
	virtual void OutsideOfRuntimeUpdate(class ECSGroup* /*_ecs*/) {}

	/**
	 * @brief ランタイム（ゲームプレイ）実行時に毎フレームシステム更新処理を行う純粋仮想関数
	 * @param _ecs 対象となるECSグループへのポインタ
	 */
	virtual void RuntimeUpdate(class ECSGroup* _ecs) = 0;

};



} /// ONEngine
