#include "TerrainCollision.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"


void TerrainCollision::OutsideOfRuntimeUpdate(ECSGroup*) {}

void TerrainCollision::RuntimeUpdate(ECSGroup* _ecs) {

	/// TerrainColliderの配列を取得＆使用中のコンポーネントがなければ終了
	ComponentArray<TerrainCollider>* terrainColliderArray = _ecs->GetComponentArray<TerrainCollider>();
	if (!terrainColliderArray || terrainColliderArray->GetUsedComponents().empty()) {
		return;
	}

	/// 他のコライダーの配列を取得
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


		/// ----- other collider との衝突判定を行う----- ///

		/// ボックスコライダーとの処理
		if (boxColliderArray) {
			for (auto& boxCollider : boxColliderArray->GetUsedComponents()) {
				if (!boxCollider || !boxCollider->enable) {
					continue;
				}

				/// state が static の場合は処理しない
				if(boxCollider->GetCollisionState() == CollisionState::Static){
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

				/// state が static の場合は処理しない
				if (sphereCollider->GetCollisionState() == CollisionState::Static) {
					continue;
				}


				if (GameEntity* sphere = sphereCollider->GetOwner()) {
					Vector3 spherePos = sphere->GetPosition();
					spherePos.y -= sphereCollider->GetRadius(); // 球の底面の位置を取得
					if (terrainCollider->IsInsideTerrain(spherePos)) {

#ifdef DEBUG_MODE
						{	/// Gizmoで値の確認
							Vector3 gradient = terrainCollider->GetGradient(sphere->GetPosition());
							const float maxSlope = 3.0f;
							float intensity = gradient.Len();
							Color color = Vector4::Lerp(Color::kBlue, Color::kRed, std::clamp(intensity / maxSlope, 0.0f, 1.0f));
							Gizmo::DrawRay(spherePos, -gradient.Normalize() * 12.0f, Color::kRed);
						}
#endif


						const Vector3 gradient = terrainCollider->GetGradient(sphere->GetPosition());
						const Vector3& prevPos = sphereCollider->GetPrevPosition();
						const Vector3 velocity = prevPos - sphere->GetPosition();
						float dot = Vector3::Dot(velocity.Normalize(), gradient.Normalize());

						float slopeAngle = GetSlopeAngle(terrainCollider, spherePos);
						float maxClimbAngle = terrainCollider->GetMaxSlopeAngle() * Mathf::Deg2Rad;

						/// 傾斜が急すぎる場合は押し上げない
						if (dot < 0.0f && slopeAngle > maxClimbAngle) {
							sphere->SetPosition(prevPos + velocity);
							spherePos = prevPos + velocity;
							spherePos.y -= sphereCollider->GetRadius();
						}

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

float TerrainCollision::GetSlopeAngle(TerrainCollider* _tCollider, const Vector3& _position) {
	Vector3 grad = _tCollider->GetGradient(_position);
	float magnitude = std::sqrt(grad.x * grad.x + grad.z * grad.z);
	return std::atan(magnitude);
}
