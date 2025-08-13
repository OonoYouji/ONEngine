#include "TerrainCollision.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"


void TerrainCollision::OutsideOfRuntimeUpdate(EntityComponentSystem*, const std::vector<class GameEntity*>&) {}

void TerrainCollision::RuntimeUpdate(EntityComponentSystem* _ecs, const std::vector<class GameEntity*>&) {

	ComponentArray<TerrainCollider>* terrainColliderArray = _ecs->GetComponentArray<TerrainCollider>();
	if (!terrainColliderArray) {
		return;
	}

	ComponentArray<BoxCollider>* boxColliderArray = _ecs->GetComponentArray<BoxCollider>();
	ComponentArray<SphereCollider>* sphereColliderArray = _ecs->GetComponentArray<SphereCollider>();

	for (auto& terrainCollider : terrainColliderArray->GetUsedComponents()) {
		if (!terrainCollider || !terrainCollider->enable) {
			continue;
		}

		/// 必要な情報がない場合はスキップする
		if (!terrainCollider->GetIsCreated()) {
			continue;
		}


		/* ----- otherコライダーとの衝突判定を行う ----- */

		/// ボックスコライダー都の処理
		if (boxColliderArray) {
			for (auto& boxCollider : boxColliderArray->GetUsedComponents()) {
				if (!boxCollider || !boxCollider->enable) {
					continue;
				}

				if (GameEntity* box = boxCollider->GetOwner()) {
					Vector3 boxPos = box->GetPosition();
					if (terrainCollider->IsInsideTerrain(boxPos)) {
						float height = terrainCollider->GetHeight(boxPos);

						/// 地形の下にいるなら押し上げる
						if (height > boxPos.y) {
							Vector3 newPos = box->GetPosition();
							newPos.y = height + (boxCollider->GetSize().y * 0.5f); // 上に押し上げる
							box->SetPosition(newPos);
						}
					}
				}
			}
		}


		/// 球コライダーとの処理
		if (sphereColliderArray) {
			for (auto& sphereCollider : sphereColliderArray->GetUsedComponents()) {
				if (!sphereCollider || !sphereCollider->enable) {
					continue;
				}

				if (GameEntity* sphere = sphereCollider->GetOwner()) {
					Vector3 spherePos = sphere->GetPosition();
					if (terrainCollider->IsInsideTerrain(spherePos)) {
						float height = terrainCollider->GetHeight(spherePos);
						/// 地形の下にいるなら押し上げる
						if (height > spherePos.y) {
							Vector3 newPos = sphere->GetPosition();
							newPos.y = height + sphereCollider->GetRadius(); // 上に押し上げる
							sphere->SetPosition(newPos);
						}
					}
				}
			}
		}

	}
}
