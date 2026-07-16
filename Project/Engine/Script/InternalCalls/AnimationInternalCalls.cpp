#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animation/AnimationPlayer.h"

using namespace ONEngine;

namespace {
    /**
     * @brief アニメーションを再生します。
     * @param nativeHandle AnimationPlayerのネイティブポインタ（キャスト用）
     */
    void Internal_Play(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) p->Play();
    }

    /**
     * @brief アニメーションを一時停止します。
     * @param nativeHandle AnimationPlayerのネイティブポインタ（キャスト用）
     */
    void Internal_Pause(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) p->Pause();
    }

    /**
     * @brief アニメーションを停止します。
     * @param nativeHandle AnimationPlayerのネイティブポインタ（キャスト用）
     */
    void Internal_Stop(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) p->Stop();
    }

    /**
     * @brief 再生するアニメーションクリップをパス指定でロード・セットします。
     * @param nativeHandle AnimationPlayerのネイティブポインタ（キャスト用）
     * @param path クリップファイルのパス（MonoString）
     */
    void Internal_SetClip(uint64_t nativeHandle, MonoString* path) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) {
            char* cstr = mono_string_to_utf8(path);
            p->SetClip(cstr);
            mono_free(cstr);
        }
    }

    /**
     * @brief アニメーションが再生中かどうかを取得します。
     * @param nativeHandle AnimationPlayerのネイティブポインタ（キャスト用）
     * @return 再生中の場合はtrue、それ以外はfalse
     */
    bool Internal_GetIsPlaying(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) return p->isPlaying;
        return false;
    }

    /**
     * @brief 現在のアニメーション再生時間を取得します。
     * @param nativeHandle AnimationPlayerのネイティブポインタ（キャスト用）
     * @return 現在の再生時間（秒）
     */
    float Internal_GetCurrentTime(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) return p->currentTime;
        return 0.0f;
    }

    /**
     * @brief アニメーションの現在の再生時間を設定します。
     * @param nativeHandle AnimationPlayerのネイティブポインタ（キャスト用）
     * @param time 設定する再生時間（秒）
     */
    void Internal_SetCurrentTime(uint64_t nativeHandle, float time) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) p->currentTime = time;
    }
}

namespace ONEngine {
    /**
     * @brief AnimationPlayer関連のC++内部関数をC#（Mono）に登録します。
     */
    void AddAnimationInternalCalls() {
        mono_add_internal_call("ONEngine.AnimationPlayer::Internal_Play", Internal_Play);
        mono_add_internal_call("ONEngine.AnimationPlayer::Internal_Pause", Internal_Pause);
        mono_add_internal_call("ONEngine.AnimationPlayer::Internal_Stop", Internal_Stop);
        mono_add_internal_call("ONEngine.AnimationPlayer::Internal_SetClip", Internal_SetClip);
        mono_add_internal_call("ONEngine.AnimationPlayer::Internal_GetIsPlaying", Internal_GetIsPlaying);
        mono_add_internal_call("ONEngine.AnimationPlayer::Internal_GetCurrentTime", Internal_GetCurrentTime);
        mono_add_internal_call("ONEngine.AnimationPlayer::Internal_SetCurrentTime", Internal_SetCurrentTime);
    }
}
