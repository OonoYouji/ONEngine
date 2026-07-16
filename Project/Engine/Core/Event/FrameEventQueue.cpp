#include "FrameEventQueue.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "GameEventData.h"
#include <string>

namespace ONEngine {

    /**
     * @brief シングルトンインスタンスを取得します。
     */
    FrameEventQueue& FrameEventQueue::GetInstance() {
        static FrameEventQueue instance;
        return instance;
    }

/**
     * @brief イベントをキューの末尾に追加します（スレッドセーフ）。
     */
    void FrameEventQueue::Enqueue(const Event& event) {
        std::lock_guard<std::mutex> lock(queueMutex_);
        queue_.push_back(event);
    }

/**
     * @brief 攻撃（当たり判定発生）イベントを生成してキューに追加します。
     */
    void FrameEventQueue::EnqueueAttackEvent(const std::string& attackName, int32_t ownerId, float damage, float radius, float duration, float offsetForward, float offsetUp) {
        Event event;
        event.type = EventType::Attack;
        AttackEventPayload payload;
        payload.attackName = attackName;
        payload.ownerId = ownerId;
        payload.damage = damage;
        payload.radius = radius;
        payload.duration = duration;
        payload.offsetForward = offsetForward;
        payload.offsetUp = offsetUp;
        event.payload = payload;
        GetInstance().Enqueue(event);
    }

/**
     * @brief エフェクト（パーティクル等）発生イベントを生成してキューに追加します。
     */
    void FrameEventQueue::EnqueueEffectEvent(const std::string& effectName, int32_t entityId, float scale, float duration) {
        Event event;
        event.type = EventType::Effect;
        EffectEventPayload payload;
        payload.effectName = effectName;
        payload.entityId = entityId;
        payload.scale = scale;
        payload.duration = duration;
        event.payload = payload;
        GetInstance().Enqueue(event);
    }

/**
     * @brief 一般的な名前付きカスタムイベントを生成してキューに追加します。
     */
    void FrameEventQueue::EnqueueNamedEvent(const std::string& eventName, int32_t entityId) {
        Event event;
        event.type = EventType::NamedEvent;
        NamedEventPayload payload;
        payload.eventName = eventName;
        payload.entityId = entityId;
        event.payload = payload;
        GetInstance().Enqueue(event);
    }

/**
     * @brief キューに溜まった全てのイベントを処理（フラッシュ）し、キューをクリアします。
     */
    void FrameEventQueue::Flush() {
        // 現在のキューをローカルにスワップして、ロック時間を最小限に抑える
        std::vector<Event> processingQueue;
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            if (queue_.empty()) {
                return;
            }
            std::swap(processingQueue, queue_);
        }

        // スワップしたキューを処理する
        for (const auto& event : processingQueue) {
            if (event.type == EventType::NamedEvent)
            {
                const auto& payload = std::get<NamedEventPayload>(event.payload);
                // Console::Log("[FrameEventQueue] Triggered Named Event: " + payload.eventName + " for Entity: " + std::to_string(payload.entityId), LogCategory::Engine);
                
                // --- 追加：特定のイベントに対する処理 ---
                if (payload.eventName == "ShowIndicator_Line") {
                    // Console::Log("[Telegraph] Spawning TelegraphLine for Entity: " + std::to_string(payload.entityId));
                }

                // 暫定：即座に完了を通知してAIを復帰させるテスト
                MonoScriptEngine::GetInstance().NotifyEventCompleted(payload.entityId, payload.eventName);
            }
            else if (event.type == EventType::Attack)
            {
                auto payload = std::get<AttackEventPayload>(event.payload);
                
                if (!payload.attackName.empty()) {
                    if (auto* def = GameEventManager::GetInstance().GetAttack(payload.attackName)) {
                        payload.damage = def->damage;
                        payload.radius = def->radius;
                        payload.duration = def->duration;
                    }
                }

                // std::string msg = "[AttackEvent] Spawn Attack: Name=" + payload.attackName + ", Damage=" + std::to_string(payload.damage);
                // Console::Log(msg, LogCategory::Application);
            }
            else if (event.type == EventType::Effect)
            {
                const auto& payload = std::get<EffectEventPayload>(event.payload);

                if (ECSGroup* group = GetEntityComponentSystemPtr()) 
                {
                    if (GameEntity* owner = group->GetEntity(payload.entityId))
                    {
                        if (auto* def = GameEventManager::GetInstance().GetEffect(payload.effectName))
                        {
                            GameEntity* effectEntity = group->GenerateEntityFromPrefab(def->effectPath);
                            if (effectEntity)
                            {
                                // 位置、向き、スケールの同期
                                effectEntity->SetPosition(owner->GetPosition());
                                effectEntity->SetRotate(owner->GetRotateQuaternion());
                                effectEntity->SetScale(Vector3::One * payload.scale);
                                
                                // C#側のハンドラがあればオーナーIDを伝える
                                // ※本来は生成直後にスクリプトのメソッドを叩く仕組みが必要だが、
                                // 現状はスクリプト側でオーナーを探すか、SetOwner等の内部呼び出しを検討
                            }
                        }
                    }
                }
            }
            else
            {   
                // 未知のイベント型
                Console::Log("Processing Unknown Event Type", LogCategory::Engine);
            }
        }
    }
}
