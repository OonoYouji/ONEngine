#pragma once

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Assets/Mesh/Skinning.h"

namespace ONEngine {

/// @brief コンパイル定数による最大数の制限
static constexpr uint32_t MAX_ANIMATION_LAYERS = 4;
static constexpr uint32_t MAX_ANIMATION_STATES_PER_LAYER = 2;

/// @brief アニメーションステート (再生中のクリップ情報)
struct AnimationState {
    uint32_t clipId = 0;
    float time = 0.0f;
    float weight = 0.0f;
    bool isLoop = true;
    float playbackSpeed = 1.0f;

    // 前フレームの再生時間 (イベント検出用)
    float prevTime = 0.0f;
};

/// @brief アニメーションレイヤー
struct AnimationLayer {
    AnimationState states[MAX_ANIMATION_STATES_PER_LAYER];
    float weight = 1.0f;
    uint32_t boneMaskHash = 0; // 0 はマスクなし

    // トランジション（クロスフェード）用
    float transitionDuration = 0.0f;
    float transitionTimer = 0.0f;
};

/// @brief Animator コンポーネント
class Animator : public IComponent {
public:
    Animator();
    ~Animator() override = default;

    /// @brief アニメーションの再生
    void Play(uint32_t _clipId, uint32_t _layerIndex = 0);

    /// @brief クロスフェード (Phase 2で詳細実装)
    void CrossFade(uint32_t _clipId, float _duration, uint32_t _layerIndex = 0);

    /// @brief 再生速度の設定
    void SetPlaybackSpeed(float _speed, uint32_t _layerIndex = 0);

    /// @brief ループ設定
    void SetLoop(bool _isLoop, uint32_t _layerIndex = 0);

    /// @brief 指定したクリップの再生時間を取得
    float GetAnimationDuration(uint32_t _clipId) const;

    /// @brief デフォルトクリップの設定
    void SetDefaultClip(uint32_t _clipId) { defaultClipId = _clipId; }
    uint32_t GetDefaultClip() const { return defaultClipId; }

public:
    /// ----- objects ----- ///
    
    // 固定長バッファによるDoD最適化
    AnimationLayer layers[MAX_ANIMATION_LAYERS];

    uint32_t defaultClipId = 0;
};

/// @brief json変換
void from_json(const nlohmann::json& _j, Animator& _animator);
void to_json(nlohmann::json& _j, const Animator& _animator);

namespace ComponentDebug {
	void AnimatorDebug(Animator* _animator);
	void AnimatorDebug(const std::vector<Animator*>& _animators);
}

/// @brief mono からのAnimator操作用関数
void Internal_Play(uint64_t _nativeHandle, uint32_t _clipId, uint32_t _layerIndex);
void Internal_CrossFade(uint64_t _nativeHandle, uint32_t _clipId, float _duration, uint32_t _layerIndex);
void Internal_SetPlaybackSpeed(uint64_t _nativeHandle, float _speed, uint32_t _layerIndex);
void Internal_SetLoop(uint64_t _nativeHandle, bool _isLoop, uint32_t _layerIndex);
float Internal_GetAnimationDuration(uint64_t _nativeHandle, uint32_t _clipId);

} // namespace ONEngine
