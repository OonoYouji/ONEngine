#include "MovementSystem.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Agent/AgentIntentComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/Core/Utility/Time/CPUTimeStamp.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"

namespace ONEngine {

/**
 * @brief コンストラクタ
 */
MovementSystem::MovementSystem() {}

/**
 * @brief 毎フレームの移動計算および位置更新処理を実行します。
 */
void MovementSystem::RuntimeUpdate(ECSGroup* _ecs) {
    CPUTimeStamp::GetInstance().BeginTimeStamp(CPUTimeStampID::PhysicsUpdate);
    if (!_ecs) {
        return;
    }

    // AIの意図に基づいて移動を処理する
    auto* intentArray = _ecs->GetComponentArray<AgentIntentComponent>();
    if (!intentArray) {
        return;
    }

    for (auto& intent : intentArray->GetUsedComponents()) {
        if (!intent || !intent->enable) {
            continue;
        }

        auto* owner = intent->GetOwner();
        if (!owner) {
            continue;
        }

        // --- 1. 回転の処理 ---
        if (intent->useDesiredRotation) {
            Quaternion currentRot = owner->GetLocalRotateQuaternion();
            
            // 現在の回転と目標の回転の間を補間する
            // rotationSpeed は 1秒あたりの補間率（簡易的な実装）
            float t = (std::min)(1.0f, intent->rotationSpeed * Time::DeltaTime());
            Quaternion newRot = Quaternion::Slerp(currentRot, intent->desiredRotation, t);
            
            owner->SetRotate(newRot);
        }

        // --- 2. 移動の処理 ---
        float targetSpeed = intent->maxSpeed;    // コンポーネントから目標速度を取得
        float acceleration = 25.0f;  // 加速度
        float deceleration = 35.0f;  // 減速度

        // 攻撃中、または移動意図がない場合は目標速度を 0 にする
        bool isMovingIntent = !intent->isAttacking && intent->desiredMoveDirection.LengthSquared() > 0.001f;
        float actualTargetSpeed = isMovingIntent ? targetSpeed : 0.0f;

        // 速度の補間（加減速）
        if (intent->currentSpeed < actualTargetSpeed) {
            intent->currentSpeed = (std::min)(actualTargetSpeed, intent->currentSpeed + acceleration * Time::DeltaTime());
        } else if (intent->currentSpeed > actualTargetSpeed) {
            intent->currentSpeed = (std::max)(actualTargetSpeed, intent->currentSpeed - deceleration * Time::DeltaTime());
        }

        // 速度がほぼ 0 なら移動処理をスキップ
        if (intent->currentSpeed < 0.001f) {
            continue;
        }
        
        // 実際の移動（現在の進行方向または意図方向を使用）
        // 簡易的に意図方向を向いていると仮定して移動
        Vector3 velocity = intent->desiredMoveDirection.Normalize() * intent->currentSpeed * Time::DeltaTime();
        Vector3 oldPos = owner->GetLocalPosition();
        Vector3 newPos = oldPos + velocity;
        owner->SetPosition(newPos);
    }
    CPUTimeStamp::GetInstance().EndTimeStamp(CPUTimeStampID::PhysicsUpdate);
}

}
