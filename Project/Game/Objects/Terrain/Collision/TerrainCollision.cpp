#include "TerrainCollision.h"

/// engine
#include "../Terrain.h"
#include "Engine/ECS/Component/Component.h"

void TerrainCollisionSystem::Update(EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) {

	/// terrainがまだ取得できていなければ terrainの取得 
	pTerrain_ = nullptr;
	for (auto& entity : _entities) {
		if (entity->GetName() == "Terrain") {
			pTerrain_ = dynamic_cast<Terrain*>(entity);
		}
	}


	/// 当たり判定を取る、とりあえず全てのentityと当たり判定を取る
	for (auto& entity : _entities) {
		if (entity->GetName() == "Terrain") { continue; } ///< terrainは除外
		/// transformの取得
		Transform* transform = entity->GetComponent<Transform>();
		ToTerrainCollider* toTerrainCollider = entity->GetComponent<ToTerrainCollider>();

		if (!transform || !toTerrainCollider) {
			continue;
		}

		/// 当たり判定を取る
		pTerrain_->Collision(transform, toTerrainCollider);

		/// prevPositionの更新
		toTerrainCollider->prevPosition_ = transform->GetPosition();
	}

}

