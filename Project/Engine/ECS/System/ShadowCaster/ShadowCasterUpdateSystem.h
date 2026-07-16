#pragma once

/// engine
#include "Engine/ECS/System/Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// ShadowCasterを更新するためのシステム
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ShadowCasterUpdateSystem
 * @brief シャドウマッピング描画（影落としオブジェクト・ライト向きカメラ）に必要なシャドウキャスター（ShadowCaster）バッファの更新・行列計算を司るシステムクラス
 */
class ShadowCasterUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	/**
	 * @brief コンストラクタ
	 */
	ShadowCasterUpdateSystem();

	/**
	 * @brief デストラクタ
	 */
	~ShadowCasterUpdateSystem() override;
	
	/**
	 * @brief エディタ非実行時のシャドウキャスター情報の更新（ビュー・プロジェクション計算など）を行います。
	 */
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief ランタイム実行時のシャドウキャスター情報の更新処理を実行します。
	 */
	void RuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief 指定されたECSグループ内の全シャドウキャスターのビュープロジェクション同期と定数バッファ同期を行う共通更新処理です。
	 */
	void Update(class ECSGroup* _ecs);

};


} /// ONEngine
