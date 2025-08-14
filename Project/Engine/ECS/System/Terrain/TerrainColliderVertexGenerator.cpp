#include "TerrainColliderVertexGenerator.h"


/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"


TerrainColliderVertexGenerator::TerrainColliderVertexGenerator(class DxManager* _dxManager)
	: pDxManager_(_dxManager) {}

void TerrainColliderVertexGenerator::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {

	ComponentArray<TerrainCollider>* colliderArray = _ecs->GetComponentArray<TerrainCollider>();
	if (!colliderArray) {
		return;
	}

	for (auto& comp : colliderArray->GetUsedComponents()) {
		if (!comp || !comp->enable) {
			continue;
		}

		if (comp->isVertexGenerationRequested_) {
			comp->AttachTerrain();
			if (Terrain* terrain = comp->GetTerrain()) {
				if (terrain->GetIsCreated()) {

					comp->isVertexGenerationRequested_ = false;
					comp->CopyVertices(pDxManager_);
					comp->isCreated_ = true;
				}
			}
		}
	}

}

void TerrainColliderVertexGenerator::RuntimeUpdate(ECSGroup*) {

}
