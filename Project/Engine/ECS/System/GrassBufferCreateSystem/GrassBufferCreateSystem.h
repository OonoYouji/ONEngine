#pragma once

/// engine
#include "Engine/ECS/System/Interface/ECSISystem.h"

/// /////////////////////////////////////////////////////
/// 草のBufferを生成するシステム
/// /////////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class GrassBufferCreateSystem
 * @brief 大量配置される草（GrassField）オブジェクト描画用のGPUバッファ（頂点/配置座標など）の生成および同期管理を司るシステムクラス
 */
class GrassBufferCreateSystem : public ECSISystem {
public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	/**
	 * @brief コンストラクタ
	 * @param _dxm GPUリソースアロケーション用の DirectX12 マネージャー
	 */
	GrassBufferCreateSystem(class DxManager* _dxm);

	/**
	 * @brief デストラクタ
	 */
	~GrassBufferCreateSystem() override;

	/**
	 * @brief エディタ非実行時の草バッファの初期化・更新チェックを行います。
	 */
	void OutsideOfRuntimeUpdate(ECSGroup* _ecs) override;

	/**
	 * @brief ランタイム実行時の草バッファの初期化・更新チェックを実行します。
	 */
	void RuntimeUpdate(ECSGroup* _ecs) override;

private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	class DxManager* pDxManager_;

};

} /// ONEngine
