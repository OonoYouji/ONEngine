#pragma once

#include "Event.h"
#include <vector>
#include <mutex>

namespace ONEngine {

    /**
     * @class FrameEventQueue
     * @brief スレッドセーフなフレーム単位イベントキュークラス。C#側から発行されたイベントなどを一時的に蓄え、フレーム末尾で一括処理します。
     */
    class FrameEventQueue {
    private:
        FrameEventQueue() = default;
        ~FrameEventQueue() = default;
        
        // Singleton pattern
        FrameEventQueue(const FrameEventQueue&) = delete;
        FrameEventQueue& operator=(const FrameEventQueue&) = delete;
        FrameEventQueue(FrameEventQueue&&) = delete;
        FrameEventQueue& operator=(FrameEventQueue&&) = delete;

        std::vector<Event> queue_;
        std::mutex queueMutex_;

    public:
        /**
         * @brief シングルトンインスタンスを取得します。
         * @return FrameEventQueueインスタンスの参照
         */
        static FrameEventQueue& GetInstance();

        /**
         * @brief イベントをキューの末尾に追加します（スレッドセーフ）。
         * @param event 追加するイベント
         */
        void Enqueue(const Event& event);

        /**
         * @brief キューに溜まった全てのイベントを処理（フラッシュ）し、キューをクリアします。
         */
        void Flush();

        // 静的なヘルパー関数
        /**
         * @brief 攻撃（当たり判定発生）イベントを生成してキューに追加します。
         */
        static void EnqueueAttackEvent(const std::string& attackName, int32_t ownerId, float damage, float radius, float duration, float offsetForward, float offsetUp);
        
        /**
         * @brief エフェクト（パーティクル等）発生イベントを生成してキューに追加します。
         */
        static void EnqueueEffectEvent(const std::string& effectName, int32_t entityId, float scale, float duration);
        
        /**
         * @brief 一般的な名前付きカスタムイベントを生成してキューに追加します。
         */
        static void EnqueueNamedEvent(const std::string& eventName, int32_t entityId);
        
        /**
         * @brief アセットリロード要求イベントを生成してキューに追加します。
         */
        static void EnqueueAssetReloadEvent(const std::string& assetPath);
        
        /**
         * @brief スクリプト（C#）ホットリロード要求イベントを生成してキューに追加します。
         */
        static void EnqueueScriptHotReloadEvent();
    };
}
