#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Effect/Effect.h"

/// /////////////////////////////////////////////////
/// EffectUpdateSystem(エフェクトの更新システム)
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class EffectUpdateSystem
 * @brief エフェクト（パーティクルエフェクトや放出オブジェクト）の放出、ライフタイム進行、ビルボード用姿勢計算、移動などの更新処理を司るシステムクラス
 */
class EffectUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	EffectUpdateSystem() = default;

	/**
	 * @brief デストラクタ
	 */
	~EffectUpdateSystem() = default;

	/**
	 * @brief 毎フレームのエフェクト発生制御と、生存している全エフェクト要素のパラメータ更新処理を実行します。
	 * @param _ecs 対象となるECSグループへのポインタ
	 */
	void RuntimeUpdate(class ECSGroup* _ecs) override;

private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	class CameraComponent* mainCamera_ = nullptr;
	Matrix4x4 matBillboard_ = Matrix4x4::kIdentity; ///< ビルボード用の行列

private:
	/// ==================================================
	/// private : methods
	/// ==================================================

	/**
	 * @brief 指定された個々のエフェクト構成要素（Element）のライフタイムや移動パラメータなどを進める内部更新処理です。
	 * @param _effect 親エフェクトコンポーネントへのポインタ
	 * @param _element 更新対象のエフェクト要素（パーティクル等）構造体へのポインタ
	 */
	void UpdateElement(Effect* _effect, Effect::Element* _element);


};


} /// ONEngine
