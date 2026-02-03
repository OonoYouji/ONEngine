#pragma once

/// engine
#include "../Interface/ECSISystem.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// ボクセル地形との衝突判定、押し戻しのシステム
/// ///////////////////////////////////////////////////
class VoxelTerrainCollision : public ECSISystem {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	VoxelTerrainCollision() = default;
	~VoxelTerrainCollision() = default;

	/// @brief ランタイムの更新処理
	/// @param _ecs ECSGroupのポインタ
	void RuntimeUpdate(class ECSGroup* _ecs) override;


private:
	/// =========================================
	/// private : objects
	/// =========================================
	
};


} /// namespace ONEngine