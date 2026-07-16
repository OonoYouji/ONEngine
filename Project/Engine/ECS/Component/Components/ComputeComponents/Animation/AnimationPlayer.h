#pragma once

/// std
#include <string>
#include <vector>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Guid/Guid.h"

struct _MonoClassField;

namespace ONEngine {

/**
 * @class AnimationPlayer
 * @brief トランスフォームの座標、カラー、スクリプト変数などの任意プロパティを、アニメーションクリップ（キーフレームデータ）に従って時間変化再生させるアニメーションプレイヤーコンポーネントクラス
 */
class AnimationPlayer : public IComponent {
public:
    /**
     * @brief コンストラクタ
     */
    AnimationPlayer();

    /**
     * @brief デストラクタ
     */
    ~AnimationPlayer() override;

    /**
     * @brief アニメーションプレイヤーの状態をデフォルトにリセットします。
     */
    void Reset() override;

    /**
     * @brief アニメーションを再生状態（isPlaying = true）にします。
     */
    void Play();

    /**
     * @brief アニメーションの再生を一時停止（isPlaying = false、時間維持）します。
     */
    void Pause();

    /**
     * @brief アニメーションの再生を停止し、時間を再生開始（時間 = 0.0f）に戻します。
     */
    void Stop();

    /**
     * @brief アニメーションクリップアセットのパス（.animファイル等）を設定します。
     */
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

    /**
     * @struct PropertyBinding
     * @brief アニメーション対象のコンポーネント変数ポインタやC#スクリプトフィールドを直接バインドするためのキャッシュ構造体
     */
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

    /**
     * @brief 所属エンティティのトランスフォームや各コンポーネント変数、C#スクリプトフィールドへの直接ポインタ参照（バインド）を構築します。
     */
    void Bind();

    /**
     * @brief 構築されたポインタ参照バインド情報をクリアし、登録されたC#側のGCハンドルも解放します。
     */
    void ClearBindings();
};

/**
 * @brief JSONからのデシリアライズ
 */
void from_json(const nlohmann::json& _j, AnimationPlayer& _a);

/**
 * @brief JSONへのシリアライズ
 */
void to_json(nlohmann::json& _j, const AnimationPlayer& _a);

namespace ComponentDebug {
    /**
     * @brief エディタ用：AnimationPlayerコンポーネントのデバッグ表示（Gui描画等）処理を行います。
     */
    void AnimationPlayerDebug(AnimationPlayer* _player);
}

} /// namespace ONEngine
