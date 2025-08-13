#pragma once

/// engine
#include "../Interface/ECSISystem.h"

#include "Engine/Core/Utility/Math/Matrix4x4.h"
#include "Engine/Graphics/Shader/ComputePipeline.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"


/// ///////////////////////////////////////////////////
/// 地形のプロシージャル生成を行うシステム
/// ///////////////////////////////////////////////////
class TerrainProcedural : public ECSISystem {
public:
	/// ========================================
	/// public : methods
	/// ========================================

	TerrainProcedural() = default;
	~TerrainProcedural() override = default;

	void OutsideOfRuntimeUpdate(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) override;
	void RuntimeUpdate(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) override;

private:
	/// ========================================
	/// private : objects
	/// ========================================

	//std::unique_ptr<ComputePipeline> computePipeline_;
	//StructuredBuffer<InstanceData> instanceDataBuffer_;

	//uint32_t instanceCount_;

};

