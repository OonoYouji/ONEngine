#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animation/AnimationPlayer.h"

using namespace ONEngine;

namespace {
    void Internal_Play(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) p->Play();
    }
    void Internal_Pause(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) p->Pause();
    }
    void Internal_Stop(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) p->Stop();
    }
    void Internal_SetClip(uint64_t nativeHandle, MonoString* path) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) {
            char* cstr = mono_string_to_utf8(path);
            p->SetClip(cstr);
            mono_free(cstr);
        }
    }
    bool Internal_GetIsPlaying(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) return p->isPlaying;
        return false;
    }
    float Internal_GetCurrentTime(uint64_t nativeHandle) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) return p->currentTime;
        return 0.0f;
    }
    void Internal_SetCurrentTime(uint64_t nativeHandle, float time) {
        if (auto* p = reinterpret_cast<AnimationPlayer*>(nativeHandle)) p->currentTime = time;
    }
}

namespace ONEngine {
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
