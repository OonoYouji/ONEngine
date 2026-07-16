#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// カメラ更新システム
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class CameraUpdateSystem
 * @brief シーン内に存在する CameraComponent のビュー行列・プロジェクション行列の更新やGPU用定数バッファ同期を行うシステムクラス
 */
class CameraUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 * @param _dxDevice GPUバッファリソース生成用の DirectX12 デバイス
	 */
	CameraUpdateSystem(class DxDevice* _dxDevice);

	/**
	 * @brief デストラクタ
	 */
	~CameraUpdateSystem() override = default;

	/**
	 * @brief エディタ非実行時のカメラ更新（ビュー調整やGPU転送など）処理を行います。
	 */
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief ランタイム実行時のカメラ更新（カメラ追従、ビュー・プロジェクション行列再計算およびGPU転送）を実行します。
	 */
	void RuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief 指定されたECSグループ内の全カメラのビュープロジェクションの再計算と定数バッファ同期を行う共通更新処理です。
	 */
	void Update(class ECSGroup* _ecs);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	class DxDevice* pDxDevice_;
	class CameraComponent* pMainCamera_;
	class CameraComponent* pMainCamera2D_;

};


} /// ONEngine
