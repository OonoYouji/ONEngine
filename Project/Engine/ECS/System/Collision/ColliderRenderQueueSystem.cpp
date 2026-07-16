#include "ColliderRenderQueueSystem.h"

using namespace ONEngine;

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"

/**
 * @brief コンストラクタ
 */
ColliderRenderQueueSystem::ColliderRenderQueueSystem() {}

/**
 * @brief エディタ非実行時のコライダー可視化処理（ギズモの描画）を行います。
 */
void ColliderRenderQueueSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	UpdateSphereCollider(_ecs->GetComponentArray<SphereCollider>());
	UpdateBoxCollider(_ecs->GetComponentArray<BoxCollider>());
}


/**
 * @brief 球体コライダー（SphereCollider）の情報からデバッグ用の球体ギズモを描画キューへ追加します。
 */
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

/**
 * @brief ボックスコライダー（BoxCollider）の情報からデバッグ用の直方体ギズモを描画キューへ追加します。
 */
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
		const Quaternion& rotate = owner->GetTransform()->GetRotate();
		// Gizmoを使って立方体を描画する
		Gizmo::DrawWireCube(position, size, rotate, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

}

