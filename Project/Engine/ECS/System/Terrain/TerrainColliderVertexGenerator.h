#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// 地形コライダーの頂点生成システム
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class TerrainColliderVertexGenerator
 * @brief CPU側の高精度な高さ計算や勾配判定のために、GPUで生成・ペイントされた地形高度UAVバッファから頂点・インデックスデータをCPUメモリ側へリードバック・同期するシステムクラス
 */
class TerrainColliderVertexGenerator : public ECSISystem {
public:
	/// ========================================
	/// public : methods
	/// ========================================

	/**
	 * @brief コンストラクタ
	 * @param _dxm GPUデータリードバック時のコマンド送信用マネージャー
	 */
	TerrainColliderVertexGenerator(class DxManager* _dxm);

	/**
	 * @brief デストラクタ
	 */
	~TerrainColliderVertexGenerator() override = default;

	/**
	 * @brief エディタ非実行時の地形編集完了時に、GPU側の地形頂点データをCPU側にコピーします。
	 */
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief ランタイム実行時のゲーム進行に追従し、リアルタイムに変更されたGPU地形頂点データをCPU側にコピーします。
	 */
	void RuntimeUpdate(class ECSGroup* _ecs) override;

private:
	/// ========================================
	/// private : objects
	/// ========================================

	class DxManager* pDxManager_;

};


} /// ONEngine
