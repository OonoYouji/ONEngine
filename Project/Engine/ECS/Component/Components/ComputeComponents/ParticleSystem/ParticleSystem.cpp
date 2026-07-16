#include "ParticleSystem.h"
#include <algorithm>

namespace ONEngine {

    /**
     * @brief 時間経過（0.0f ~ 1.0f）に応じたカラーグラデーション値を評価・補間算出します。
     * @param time 評価時間割合 (0.0f - 1.0f)
     * @return 補間されたカラー値 (RGBA)
     */
    Color ParticleSystemGradient::Evaluate(float time) const {
        if (colorKeys.empty() && alphaKeys.empty()) return Color::kWhite;

        Color result = Color::kWhite;

        // Color
        if (!colorKeys.empty()) {
            if (time <= colorKeys.front().time) {
                result.r = colorKeys.front().color.r;
                result.g = colorKeys.front().color.g;
                result.b = colorKeys.front().color.b;
            } else if (time >= colorKeys.back().time) {
                result.r = colorKeys.back().color.r;
                result.g = colorKeys.back().color.g;
                result.b = colorKeys.back().color.b;
            } else {
                for (size_t i = 0; i < colorKeys.size() - 1; ++i) {
                    if (time >= colorKeys[i].time && time <= colorKeys[i + 1].time) {
                        float t = (time - colorKeys[i].time) / (colorKeys[i + 1].time - colorKeys[i].time);
                        result.r = colorKeys[i].color.r + (colorKeys[i + 1].color.r - colorKeys[i].color.r) * t;
                        result.g = colorKeys[i].color.g + (colorKeys[i + 1].color.g - colorKeys[i].color.g) * t;
                        result.b = colorKeys[i].color.b + (colorKeys[i + 1].color.b - colorKeys[i].color.b) * t;
                        break;
                    }
                }
            }
        }

        // Alpha
        if (!alphaKeys.empty()) {
            if (time <= alphaKeys.front().time) {
                result.a = alphaKeys.front().alpha;
            } else if (time >= alphaKeys.back().time) {
                result.a = alphaKeys.back().alpha;
            } else {
                for (size_t i = 0; i < alphaKeys.size() - 1; ++i) {
                    if (time >= alphaKeys[i].time && time <= alphaKeys[i + 1].time) {
                        float t = (time - alphaKeys[i].time) / (alphaKeys[i + 1].time - alphaKeys[i].time);
                        result.a = alphaKeys[i].alpha + (alphaKeys[i + 1].alpha - alphaKeys[i].alpha) * t;
                        break;
                    }
                }
            }
        } else if (!colorKeys.empty()) {
            // If no alpha keys, maybe use alpha from color keys? 
            // Usually Shuriken separates them. We'll default to 1.0 if no alpha keys.
            result.a = 1.0f; 
        }

        return result;
    }

    /**
     * @brief アニメーションカーブ上の時間に対応する浮動小数点値を線形補間により算出します。
     * @param time 評価時間
     * @return カーブの値
     */
    float AnimationCurve::Evaluate(float time) const {
        if (keys.empty()) return 1.0f;

        if (time <= keys.front().time) return keys.front().value;
        if (time >= keys.back().time) return keys.back().value;

        for (size_t i = 0; i < keys.size() - 1; ++i) {
            if (time >= keys[i].time && time <= keys[i + 1].time) {
                float t = (time - keys[i].time) / (keys[i + 1].time - keys[i].time);
                return keys[i].value + (keys[i + 1].value - keys[i].value) * t;
            }
        }

        return 1.0f;
    }

    /**
     * @brief コンストラクタ
     */
    ParticleSystem::ParticleSystem() {
        // Initialize default state
    }

    /**
     * @brief パーティクルのシミュレーション・放出を開始します。
     */
    void ParticleSystem::Play() {
        isPlaying_ = true;
        isPaused_ = false;
        playbackTime_ = 0.0f;
        aliveCount = 0;
        emitAccumulator = 0.0f;
        std::fill(burstCycleCounts.begin(), burstCycleCounts.end(), 0);
    }

    /**
     * @brief パーティクルのシミュレーションを停止し、新規放出を止めます。
     */
    void ParticleSystem::Stop() {
        isPlaying_ = false;
        isPaused_ = false;
    }

    /**
     * @brief 現在生存しているすべてのパーティクルを強制消去し、初期化します。
     */
    void ParticleSystem::Clear() {
        playbackTime_ = 0.0f;
        // Also clear GPU buffers in the future
    }

    /**
     * @brief シミュレーションを一時停止します。
     */
    void ParticleSystem::Pause() {
        isPaused_ = true;
    }

}
