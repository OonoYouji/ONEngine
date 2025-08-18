#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// ///////////////////////////////////////////////////
/// 地形コライダーの頂点生成システム
/// ///////////////////////////////////////////////////
class TerrainColliderVertexGenerator : public ECSISystem {
public:
	/// ========================================
	/// public : methods
	/// ========================================

	TerrainColliderVertexGenerator(class DxManager* _dxManager);
	~TerrainColliderVertexGenerator() override = default;

	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
	void RuntimeUpdate(class ECSGroup* _ecs) override;

private:
	/// ========================================
	/// private : objects
	/// ========================================

	class DxManager* pDxManager_;

};

