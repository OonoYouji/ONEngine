#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Assets/Mesh/Skinning.h"

namespace ONEngine {

/// @brief コンパイル定数による最大数の制限
static constexpr uint32_t MAX_ANIMATION_LAYERS = 4;
static constexpr uint32_t MAX_ANIMATION_STATES_PER_LAYER = 2;

class IAnimationLayerState {
public:
    virtual ~IAnimationLayerState() = default;
    virtual void Update(struct AnimationLayer& layer, float deltaTime, const std::unordered_map<uint32_t, struct AnimationClip>& clips) = 0;
    virtual void Play(struct AnimationLayer& layer, uint32_t clipId) = 0;
    virtual void CrossFade(struct AnimationLayer& layer, uint32_t clipId, float duration) = 0;
};

class AnimationLayerPlayingState : public IAnimationLayerState {
public:
    static AnimationLayerPlayingState* GetInstance();
    void Update(struct AnimationLayer& layer, float deltaTime, const std::unordered_map<uint32_t, struct AnimationClip>& clips) override;
    void Play(struct AnimationLayer& layer, uint32_t clipId) override;
    void CrossFade(struct AnimationLayer& layer, uint32_t clipId, float duration) override;
};

class AnimationLayerTransitionState : public IAnimationLayerState {
public:
    static AnimationLayerTransitionState* GetInstance();
    void Update(struct AnimationLayer& layer, float deltaTime, const std::unordered_map<uint32_t, struct AnimationClip>& clips) override;
    void Play(struct AnimationLayer& layer, uint32_t clipId) override;
    void CrossFade(struct AnimationLayer& layer, uint32_t clipId, float duration) override;
};

/**
 * @struct AnimationState
 * @brief 再生中のアニメーションクリップに関する状態（再生時間、ブレンドウェイト、ループフラグ、再生速度など）を表す構造体
 */
struct AnimationState {
    uint32_t clipId = 0;
    float time = 0.0f;
    float weight = 0.0f;
    bool isLoop = true;
    float playbackSpeed = 1.0f;

    // 前フレームの再生時間 (イベント検出用)
    float prevTime = 0.0f;
};

/**
 * @struct AnimationLayer
 * @brief アニメーションの複数レイヤー合成用の情報を表す構造体。各レイヤーで再生するステートやレイヤー全体のウェイト、ボーンマスク情報を保持します。
 */
struct AnimationLayer {
    AnimationLayer();
    AnimationState states[MAX_ANIMATION_STATES_PER_LAYER];
    float weight = 1.0f;
    uint32_t boneMaskHash = 0; // 0 はマスクなし

    // トランジション（クロスフェード）用
    float transitionDuration = 0.0f;
    float transitionTimer = 0.0f;

    IAnimationLayerState* currentState = nullptr;
};

/**
 * @class Animator
 * @brief 骨格（スキンメッシュ）アニメーションの再生制御、ステート遷移、クロスフェード、レイヤー間ブレンドを管理するコンポーネントクラス
 */
class Animator : public IComponent {
public:
    /**
     * @brief コンストラクタ
     */
    Animator();

    /**
     * @brief デストラクタ
     */
    ~Animator() override = default;

    /**
     * @brief 指定したアニメーションクリップを即座に再生します。
     * @param _clipId 再生するクリップID
     * @param _layerIndex 再生対象のレイヤーインデックス（デフォルトは0）
     */
    void Play(uint32_t _clipId, uint32_t _layerIndex = 0);

    /**
     * @brief 指定した秒数（duration）をかけて、別のアニメーションクリップへクロスフェード（ブレンド）遷移を行います。
     * @param _clipId 遷移先クリップID
     * @param _duration クロスフェードにかける秒数
     * @param _layerIndex 遷移対象のレイヤーインデックス（デフォルトは0）
     */
    void CrossFade(uint32_t _clipId, float _duration, uint32_t _layerIndex = 0);

    /**
     * @brief 指定レイヤーのアニメーション再生速度を設定します。
     * @param _speed 再生倍率（1.0fが等倍）
     * @param _layerIndex 対象レイヤーインデックス（デフォルトは0）
     */
    void SetPlaybackSpeed(float _speed, uint32_t _layerIndex = 0);

    /**
     * @brief 指定レイヤーのアニメーションのループ再生を設定します。
     * @param _isLoop ループ再生する場合は true、1回のみ再生の場合は false
     * @param _layerIndex 対象レイヤーインデックス（デフォルトは0）
     */
    void SetLoop(bool _isLoop, uint32_t _layerIndex = 0);

    /**
     * @brief 指定されたアニメーションクリップの総再生長さ（秒）を取得します。
     * @param _clipId クリップID
     */
    float GetAnimationDuration(uint32_t _clipId) const;

    /**
     * @brief 開始時に自動再生するデフォルトクリップIDを設定します。
     */
    void SetDefaultClip(uint32_t _clipId) { defaultClipId = _clipId; }

    /**
     * @brief 開始時に自動再生するデフォルトクリップIDを取得します。
     */
    uint32_t GetDefaultClip() const { return defaultClipId; }

public:
    /// ----- objects ----- ///
    
    // 固定長バッファによるDoD最適化
    AnimationLayer layers[MAX_ANIMATION_LAYERS];

    uint32_t defaultClipId = 0;
};

/**
 * @brief JSONからのデシリアライズ
 */
void from_json(const nlohmann::json& _j, Animator& _animator);

/**
 * @brief JSONへのシリアライズ
 */
void to_json(nlohmann::json& _j, const Animator& _animator);

namespace ComponentDebug {
    /**
     * @brief エディタ用：Animatorコンポーネントのデバッグ表示（Gui描画等）処理を行います。
     */
	void AnimatorDebug(Animator* _animator);

    /**
     * @brief エディタ用：複数Animatorコンポーネントの一括デバッグ表示処理を行います。
     */
	void AnimatorDebug(const std::vector<Animator*>& _animators);
}

/// @brief mono からのAnimator操作用関数

/**
 * @brief C#（Mono）インターフェース用：指定クリップの即座再生
 */
void Internal_Play(uint64_t _nativeHandle, uint32_t _clipId, uint32_t _layerIndex);

/**
 * @brief C#（Mono）インターフェース用：指定クリップへのクロスフェード開始
 */
void Internal_CrossFade(uint64_t _nativeHandle, uint32_t _clipId, float _duration, uint32_t _layerIndex);

/**
 * @brief C#（Mono）インターフェース用：再生速度の設定
 */
void Internal_SetPlaybackSpeed(uint64_t _nativeHandle, float _speed, uint32_t _layerIndex);

/**
 * @brief C#（Mono）インターフェース用：ループ再生設定の変更
 */
void Internal_SetLoop(uint64_t _nativeHandle, bool _isLoop, uint32_t _layerIndex);

/**
 * @brief C#（Mono）インターフェース用：アニメーションの総長さ（秒）取得
 */
float Internal_GetAnimationDuration(uint64_t _nativeHandle, uint32_t _clipId);

} // namespace ONEngine
