#include "ColliderRenderQueueSystem.h"

/// engine
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

ColliderRenderQueueSystem::ColliderRenderQueueSystem() {

}

void ColliderRenderQueueSystem::Update([[maybe_unused]] EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) {
	UpdateSphereCollider(_entities);
	UpdateBoxCollider(_entities);
}


void ColliderRenderQueueSystem::UpdateSphereCollider(const std::vector<class IEntity*>& _entities) {

	std::vector<SphereCollider*> sphereColliders;
	for (auto& entity : _entities) {
		SphereCollider* sphereCollider = entity->GetComponent<SphereCollider>();
		if (sphereCollider && sphereCollider->enable) {
			sphereColliders.push_back(sphereCollider);
		}
	}

	if (sphereColliders.empty()) {
		return; ///< 描画するスフィアコライダーがない場合は何もしない
	}


	/// gizmoを使って表示する
	for (auto& sphereCollider : sphereColliders) {
		if (!sphereCollider) {
			continue; // 無効なコライダーはスキップ
		}

		IEntity* owner = sphereCollider->GetOwner();
		if (!owner) {
			continue; // オーナーが無効な場合はスキップ
		}

		Vec3 position = owner->GetPosition();
		float radius = sphereCollider->GetRadius();
		// Gizmoを使って球体を描画する
		Gizmo::DrawWireSphere(position, radius, Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // 赤色で描画
	}

}

void ColliderRenderQueueSystem::UpdateBoxCollider(const std::vector<class IEntity*>& _entities) {

	std::vector<BoxCollider*> boxColliders;
	for (auto& entity : _entities) {
		BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
		if (boxCollider && boxCollider->enable) {
			boxColliders.push_back(boxCollider);
		}
	}

	if( boxColliders.empty()) {
		return; ///< 描画するボックスコライダーがない場合は何もしない
	}

	/// gizmoを使って表示する
	for (auto& boxCollider : boxColliders) {
		if (!boxCollider) {
			continue; // 無効なコライダーはスキップ
		}
		IEntity* owner = boxCollider->GetOwner();
		if (!owner) {
			continue; // オーナーが無効な場合はスキップ
		}
		Vec3 position = owner->GetPosition();
		Vec3 size = boxCollider->GetSize();
		// Gizmoを使って立方体を描画する
		Gizmo::DrawWireCube(position, size, Vec4(0.0f, 1.0f, 0.0f, 1.0f)); // 緑色で描画
	}

}

