#pragma once

#include "Event.h"
#include <vector>
#include <mutex>

namespace ONEngine {

    /// <summary>
    /// フレーム単位でのイベントキュー。
    /// 主にC#側から発行されたイベントをC++側で受け取り、フレームの最後に一括処理するために使用する。
    /// スレッドセーフ。
    /// </summary>
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
        /// <summary>
        /// シングルトンインスタンスを取得する
        /// </summary>
        static FrameEventQueue& GetInstance();

        /// <summary>
        /// イベントをキューの末尾に追加する
        /// </summary>
        /// <param name="event">追加するイベント</param>
        void Enqueue(const Event& event);

        /// <summary>
        /// キューに溜まった全てのイベントを処理する
        /// </summary>
        void Flush();

        // 静的なヘルパー関数
        static void EnqueueAttackEvent(const std::string& attackName, int32_t ownerId, float damage, float radius, float duration, float offsetForward, float offsetUp);
        static void EnqueueEffectEvent(const std::string& effectName, int32_t entityId, float scale, float duration);
        static void EnqueueNamedEvent(const std::string& eventName, int32_t entityId);
        static void EnqueueAssetReloadEvent(const std::string& assetPath);
        static void EnqueueScriptHotReloadEvent();
    };
}
