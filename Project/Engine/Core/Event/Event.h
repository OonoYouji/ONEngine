#pragma once

#include <variant>
#include <cstdint>
#include <string>

namespace ONEngine
{
    // イベントのペイロード（具体的なデータ）を定義
    // エンティティIDを持つイベント
    struct EntityEventPayload
    {
        int32_t entityId;
    };

    // 名前付きイベント（文字列）とエンティティIDを持つペイロード
    struct NamedEventPayload
    {
        std::string eventName;
        int32_t entityId;
    };

    // エフェクト（パーティクル・視覚演出）用ペイロード
    struct EffectEventPayload
    {
        std::string effectName; // プリセット名
        int32_t entityId;       // 発生源エンティティ
        float scale;            // スケール倍率
        float duration;         // 持続時間
    };

    // 攻撃（当たり判定生成）イベント用ペイロード
    struct AttackEventPayload
    {
        std::string attackName; // プリセット名（空なら以下のパラメータを使用）
        int32_t ownerId;        // 攻撃者ID
        float damage;           // ダメージ量
        float radius;           // 半径
        float duration;         // 持続時間
        float offsetForward;    // 前方オフセット
        float offsetUp;         // 上方オフセット
    };

    // アセットリロード用ペイロード
    struct AssetReloadPayload
    {
        std::string assetPath;
    };

    // イベントの種類を定義
    enum class EventType : uint8_t
    {
        TestEvent,
        NamedEvent, // 文字列ベースのイベント
        Attack,     // 攻撃発生イベント
        Effect,     // エフェクト発生イベント
        AssetReload, // アセットリロード
        ScriptHotReload, // スクリプトホットリロード
    };

    // イベント本体
    // std::variantを使って、型安全なペイロードを持つ
    struct Event
    {
        EventType type;
        std::variant<
            EntityEventPayload,
            NamedEventPayload,
            EffectEventPayload,
            AttackEventPayload,
            AssetReloadPayload
        > payload;
    };
}
