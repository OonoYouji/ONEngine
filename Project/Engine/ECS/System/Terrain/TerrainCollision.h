#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/Core/Utility/Math/Vector3.h"

/// ///////////////////////////////////////////////////
/// 地形との衝突判定、押し戻しのシステム
/// ///////////////////////////////////////////////////
namespace ONEngine {

class TerrainCollision : public ECSISystem {
public:
	/// ========================================
	/// public : methods
	/// ========================================

	TerrainCollision() = default;
	~TerrainCollision() override = default;

	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
	void RuntimeUpdate(class ECSGroup* _ecs) override;

	/// @brief 今いる地点の傾斜角を取得する
	/// @param _tCollider TerrainColliderのポインタ
	/// @param _position world座標
	/// @return 勾配
	float GetSlopeAngle(class TerrainCollider* _tCollider, const Vector3& _position);

private:
	/// ========================================
	/// private : objects
	/// ========================================

};


} /// ONEngine
