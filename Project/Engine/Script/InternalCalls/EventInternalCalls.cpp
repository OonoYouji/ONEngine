#include "EventInternalCalls.h"
#include "Engine/Core/Event/FrameEventQueue.h"
#include <mono/jit/jit.h>
#include <string>

namespace ONEngine {

    /**
     * @brief C#からイベントをC++のキューに追加するための内部呼び出し
     * @param eventType イベントの種類
     * @param entityId 関連するエンティティのID
     */
    static void Internal_EnqueueEntityEvent(EventType eventType, int32_t entityId)
    {
        Event e;
        e.type = eventType;
        e.payload = EntityEventPayload{ entityId };
        FrameEventQueue::GetInstance().Enqueue(e);
    }

    /**
     * @brief C#から名前付きイベントをC++のキューに追加するための内部呼び出し
     * @param eventName イベント名
     * @param entityId 関連するエンティティのID
     */
    static void Internal_EnqueueNamedEvent(MonoString* eventName, int32_t entityId)
    {
        char* name = mono_string_to_utf8(eventName);
        
        Event e;
        e.type = EventType::NamedEvent;
        e.payload = NamedEventPayload{ std::string(name), entityId };
        FrameEventQueue::GetInstance().Enqueue(e);

        mono_free(name);
    }

    /**
     * @brief C#から攻撃イベントを発行するための内部呼び出し
     * @param attackName 攻撃の名前
     * @param ownerId 発行元エンティティのID
     * @param damage 与えるダメージ
     * @param radius 攻撃の半径
     * @param duration 攻撃の持続時間
     * @param offsetForward 前方へのオフセット
     * @param offsetUp 上方へのオフセット
     */
    static void Internal_EnqueueAttackEvent(MonoString* attackName, int32_t ownerId, float damage, float radius, float duration, float offsetForward, float offsetUp)
    {
        char* name = mono_string_to_utf8(attackName);

        Event e;
        e.type = EventType::Attack;
        e.payload = AttackEventPayload{ std::string(name), ownerId, damage, radius, duration, offsetForward, offsetUp };
        FrameEventQueue::GetInstance().Enqueue(e);

        mono_free(name);
    }

    /**
     * @brief C#からエフェクトイベントを発行するための内部呼び出し
     * @param effectName エフェクト名
     * @param entityId 対象のエンティティID
     * @param scale エフェクトのスケール
     * @param duration エフェクトの持続時間
     */
    static void Internal_EnqueueEffectEvent(MonoString* effectName, int32_t entityId, float scale, float duration)
    {
        char* name = mono_string_to_utf8(effectName);

        Event e;
        e.type = EventType::Effect;
        e.payload = EffectEventPayload{ std::string(name), entityId, scale, duration };
        FrameEventQueue::GetInstance().Enqueue(e);

        mono_free(name);
    }

    /**
     * @brief イベントキューイング用のC++内部関数をC#（Mono）にバインドします。
     */
    void AddEventInternalCalls()
    {
        mono_add_internal_call("FrameEvent::Internal_EnqueueEntityEvent", (void*)Internal_EnqueueEntityEvent);
        mono_add_internal_call("FrameEvent::Internal_EnqueueNamedEvent", (void*)Internal_EnqueueNamedEvent);
        mono_add_internal_call("FrameEvent::Internal_EnqueueAttackEvent", (void*)Internal_EnqueueAttackEvent);
        mono_add_internal_call("FrameEvent::Internal_EnqueueEffectEvent", (void*)Internal_EnqueueEffectEvent);
    }
}
