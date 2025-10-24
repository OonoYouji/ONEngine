#include "ColliderRenderQueueSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

ColliderRenderQueueSystem::ColliderRenderQueueSystem() {}

void ColliderRenderQueueSystem::RuntimeUpdate(ECSGroup* _ecs) {
	UpdateSphereCollider(_ecs->GetComponentArray<SphereCollider>());
	UpdateBoxCollider(_ecs->GetComponentArray<BoxCollider>());
}


void ColliderRenderQueueSystem::UpdateSphereCollider(ComponentArray<SphereCollider>* _sphereColliderArray) {

	/// SphereColliderが存在するか確認(空ならreturn)
	if (!_sphereColliderArray || _sphereColliderArray->GetUsedComponents().empty()) {
		return;
	}


	for (auto& sphereCollider : _sphereColliderArray->GetUsedComponents()) {
		if (!sphereCollider) {
			continue; // 無効なコライダーはスキップ
		}

		GameEntity* owner = sphereCollider->GetOwner();
		if (!owner) {
			continue; // オーナーが無効な場合はスキップ
		}

		Vector3 position = owner->GetPosition();
		float radius = sphereCollider->GetRadius();
		// Gizmoを使って球体を描画する
		Gizmo::DrawWireSphere(position, radius, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	}

}

void ColliderRenderQueueSystem::UpdateBoxCollider(ComponentArray<BoxCollider>* _boxColliderArray) {

	/// BoxColliderが存在するか確認(空ならreturn)
	if (!_boxColliderArray || _boxColliderArray->GetUsedComponents().empty()) {
		return;
	}

	/// gizmoを使って表示する
	for (auto& boxCollider : _boxColliderArray->GetUsedComponents()) {
		if (!boxCollider) {
			continue; // 無効なコライダーはスキップ
		}
		GameEntity* owner = boxCollider->GetOwner();
		if (!owner) {
			continue; // オーナーが無効な場合はスキップ
		}

		const Vector3 position = owner->GetPosition();
		const Vector3& size = boxCollider->GetSize();
		// Gizmoを使って立方体を描画する
		Gizmo::DrawWireCube(position, size, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

}

