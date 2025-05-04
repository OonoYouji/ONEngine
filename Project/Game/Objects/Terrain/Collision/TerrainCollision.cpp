#include "TerrainCollision.h"

/// engine
#include "../Terrain.h"
#include "Engine/ECS/Component/Component.h"

void TerrainCollisionSystem::Update(EntityComponentSystem* _ecs) {

	/// terrainがまだ取得できていなければ terrainの取得 
	if (!pTerrain_) {
		for (auto& entity : _ecs->GetEntities()) {
			if (entity->GetName() == "Terrain") {
				pTerrain_ = dynamic_cast<Terrain*>(entity.get());
			}
		}
	}



	/// 当たり判定を取る、とりあえず全てのentityと当たり判定を取る
	for (auto& entity : _ecs->GetEntities()) {
		if (entity->GetName() == "Terrain") { continue; } ///< terrainは除外
		/// transformの取得
		Transform* transform = entity->GetComponent<Transform>();
		ToTerrainCollider* toTerrainCollider = entity->GetComponent<ToTerrainCollider>();
		
		if (!transform || !toTerrainCollider) { 
			continue; 
		}

		/// 当たり判定を取る
		pTerrain_->Collision(transform, toTerrainCollider);
	}

}
