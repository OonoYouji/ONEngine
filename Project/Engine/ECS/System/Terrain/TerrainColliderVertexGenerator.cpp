#include "TerrainColliderVertexGenerator.h"

using namespace ONEngine;


/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"


/**
 * @brief コンストラクタ
 */
TerrainColliderVertexGenerator::TerrainColliderVertexGenerator(class DxManager* _dxm)
	: pDxManager_(_dxm) {}

/**
 * @brief エディタ非実行時の地形編集完了時に、GPU側の地形頂点データをCPU側にコピーします。
 */
void TerrainColliderVertexGenerator::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {

	ComponentArray<TerrainCollider>* colliderArray = _ecs->GetComponentArray<TerrainCollider>();
	if (!colliderArray || colliderArray->GetUsedComponents().empty()) {
		return;
	}

	for (auto& comp : colliderArray->GetUsedComponents()) {
		if (!comp || !comp->enable) {
			continue;
		}

		/// 頂点情報の生成が要求されている場合、地形から頂点情報をコピーしコライダーとして扱う
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

/**
 * @brief ランタイム実行時のゲーム進行に追従し、リアルタイムに変更されたGPU地形頂点データをCPU側にコピーします。
 */
void TerrainColliderVertexGenerator::RuntimeUpdate(ECSGroup*) {

}
