#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/Core/Utility/Math/Vector3.h"

/// ///////////////////////////////////////////////////
/// 地形との衝突判定、押し戻しのシステム
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class TerrainCollision
 * @brief キャラクターなどのエンティティが地形（高度）上に追従して立つように、高さ補正（Y軸方向への接地・押し戻し）や勾配・法線ベクトルの算出を行うシステムクラス
 */
class TerrainCollision : public ECSISystem {
public:
	/// ========================================
	/// public : methods
	/// ========================================

	/**
	 * @brief コンストラクタ
	 */
	TerrainCollision() = default;

	/**
	 * @brief デストラクタ
	 */
	~TerrainCollision() override = default;

	/**
	 * @brief エディタ非実行時の地形接地・高さ補正処理を行います。
	 */
	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief ランタイム実行時の全アクティブエンティティに対する地形接地・高さ補正計算を実行します。
	 */
	void RuntimeUpdate(class ECSGroup* _ecs) override;

	/**
	 * @brief 指定したワールド座標直下にある地形の傾斜角（法線ベクトルから算出）を取得します。
	 * @param _tCollider 対象地形コライダーへのポインタ
	 * @param _position 対象のワールド座標
	 * @return 地面の傾斜角（度数法）
	 */
	float GetSlopeAngle(class TerrainCollider* _tCollider, const Vector3& _position);

private:
	/// ========================================
	/// private : objects
	/// ========================================

};


} /// ONEngine
