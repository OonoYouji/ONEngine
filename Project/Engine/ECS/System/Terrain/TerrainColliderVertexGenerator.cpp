#include "TerrainColliderVertexGenerator.h"


/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"


TerrainColliderVertexGenerator::TerrainColliderVertexGenerator(class DxManager* _dxManager)
	: pDxManager_(_dxManager) {}

void TerrainColliderVertexGenerator::OutsideOfRuntimeUpdate(EntityComponentSystem* _ecs, const std::vector<class IEntity*>&) {

	ComponentArray<TerrainCollider>* colliderArray = _ecs->GetComponentArray<TerrainCollider>();
	if (!colliderArray) {
		return;
	}

	for (auto& comp : colliderArray->GetUsedComponents()) {
		if (!comp || !comp->enable) {
			continue;
		}

		if (comp->isVertexGenerationRequested_) {
			comp->isVertexGenerationRequested_ = false;
			comp->CopyVertices(pDxManager_);
		}
	}

}

void TerrainColliderVertexGenerator::RuntimeUpdate(EntityComponentSystem*, const std::vector<class IEntity*>&) {

}
