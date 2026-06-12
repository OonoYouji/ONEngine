#pragma once

/// std
#include <string>
#include <vector>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Guid/Guid.h"

struct _MonoClassField;

namespace ONEngine {

class AnimationPlayer : public IComponent {
public:
    AnimationPlayer();
    ~AnimationPlayer() override;

    void Reset() override;

    /// @brief アニメーションを再生する
    void Play();
    /// @brief アニメーションを一時停止する
    void Pause();
    /// @brief アニメーションを停止する
    void Stop();

    /// @brief 使用するアニメーションクリップを設定する
    void SetClip(const std::string& _path);

    /// ===============================================
    /// public : objects
    /// ===============================================

    std::string clipPath;
    float currentTime = 0.0f;
    float speed = 1.0f;
    bool isPlaying = false;
    bool isLooping = true;
    bool autoPlay = true;
    bool shouldApplyOnce = false; // Stop時などに一度だけ値を適用するためのフラグ

    /// アニメーション対象のプロパティを解決したキャッシュ
    struct PropertyBinding {
        IComponent* targetComponent;
        std::string propertyPath;
        void* dataPtr = nullptr; // 直接値を書き換えるためのポインタ
        
        // --- C# Script Support ---
        uint32_t monoGcHandle = 0; // MonoObjectのGCハンドル (0は無効)
        struct _MonoClassField* monoField = nullptr;

        enum class Type {
            None = 0,
            Float, Vector2, Vector3, Vector4, Bool, Int,
            TransformRotationEuler, // 特殊対応：Vector3 Euler -> Quaternion
            ScriptVar, // 従来のVariablesコンポーネント経由 (後方互換用)
            CSField    // C#スクリプトのフィールド直書き
        } type = Type::None;
        std::string scriptGroupName; // ScriptVar/CSField のクラス名
        std::string scriptVarName;   // ScriptVar/CSField の変数名
    };
    std::vector<PropertyBinding> bindings;
    bool isBound = false;

    /// @brief エンティティのコンポーネントに対してプロパティをバインドする
    void Bind();

    /// @brief バインド情報をクリアし、GCハンドルを解放する
    void ClearBindings();
};

void from_json(const nlohmann::json& _j, AnimationPlayer& _a);
void to_json(nlohmann::json& _j, const AnimationPlayer& _a);

namespace ComponentDebug {
    void AnimationPlayerDebug(AnimationPlayer* _player);
}

} /// namespace ONEngine
