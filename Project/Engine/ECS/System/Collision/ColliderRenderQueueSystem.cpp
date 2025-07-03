#include "ColliderRenderQueueSystem.h"

/// engine
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

ColliderRenderQueueSystem::ColliderRenderQueueSystem() {

}

void ColliderRenderQueueSystem::Update(EntityComponentSystem* _ecs) {
	UpdateSphereCollider(_ecs);
	UpdateBoxCollider(_ecs);
}

void ColliderRenderQueueSystem::UpdateSphereCollider(EntityComponentSystem* _ecs) {
	ComponentArray<SphereCollider>* sphereColliders = _ecs->GetComponentArray<SphereCollider>();
	if (!sphereColliders) {
		return; // SphereColliderが存在しない場合は何もしない
	}

	/// gizmoを使って表示する
	for (auto& sphereCollider : sphereColliders->GetUsedComponents()) {
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

void ColliderRenderQueueSystem::UpdateBoxCollider(EntityComponentSystem* _ecs) {
	ComponentArray<BoxCollider>* boxColliders = _ecs->GetComponentArray<BoxCollider>();
	if (!boxColliders) {
		return; // BoxColliderが存在しない場合は何もしない
	}

	/// gizmoを使って表示する
	for (auto& boxCollider : boxColliders->GetUsedComponents()) {
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

