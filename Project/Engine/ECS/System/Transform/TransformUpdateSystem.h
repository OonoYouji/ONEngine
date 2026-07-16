#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// Transformの行列を更新するシステム
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class TransformUpdateSystem
 * @brief すべてのエンティティが持つ位置・回転・拡縮（Transform）情報から、3Dグラフィックス描画や物理計算に必要なワールド変換行列（matWorld）を毎フレーム一括計算・更新するシステムクラス
 */
class TransformUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	/**
	 * @brief コンストラクタ
	 */
	TransformUpdateSystem() = default;

	/**
	 * @brief デストラクタ
	 */
	~TransformUpdateSystem() override = default;
	
	/**
	 * @brief エディタ非実行時のワールド行列の再計算および階層伝搬更新を行います。
	 */
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief ランタイム実行時の全アクティブエンティティのワールド行列の再計算および親子関係の更新を実行します。
	 */
	void RuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief 指定されたECSグループ内の全エンティティの Transform のワールド行列を更新する共通処理です。
	 */
	void Update(class ECSGroup* _ecs);
};


} /// ONEngine
