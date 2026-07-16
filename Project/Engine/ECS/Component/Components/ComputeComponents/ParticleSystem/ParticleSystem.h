#pragma once

#include "../../Interface/IComponent.h"
#include "ParticleSystemData.h"
#include "Engine/Core/Utility/Math/Matrix4x4.h"

namespace ONEngine {

    // CPU-side particle state
    struct Particle {
        Vector3 position;
        Vector3 velocity;
        Color color;
        float startLifetime;
        float remainingLifetime;
        float size;
        float rotation;
        Color startColor;
        float startSize;
        Vector3 baseVelocity;
        float randomValue; // 出生時に決定される [0, 1] の乱数
        uint32_t simulationSpace; // 0: World, 1: Local
    };

    /**
     * @class ParticleSystem
     * @brief 複数の微粒子（パーティクル）の生成、移動、状態変化、寿命管理などを、メイン、エミッション、形状、カラー変化、サイズ変化等の各種モジュール設定をもとにシミュレーションするコンポーネントクラス
     */
    class ParticleSystem : public IComponent {
    public:
        /**
         * @brief コンストラクタ
         */
        ParticleSystem();

        /**
         * @brief デストラクタ
         */
        ~ParticleSystem() override = default;

        // --- Controls ---
        
        /**
         * @brief パーティクルのシミュレーション・放出を開始します。
         */
        void Play();

        /**
         * @brief パーティクルのシミュレーションを停止し、新規放出を止めます。
         */
        void Stop();

        /**
         * @brief 現在生存しているすべてのパーティクルを強制消去し、初期化します。
         */
        void Clear();

        /**
         * @brief シミュレーションを一時停止します。
         */
        void Pause();

        /**
         * @brief シミュレーション経過時間を進めます。
         * @param _dt デルタ時間（秒）
         */
        void UpdateTime(float _dt) { playbackTime_ += _dt; }

        /**
         * @brief シミュレーション経過時間を指定値にリセットします。
         */
        void ResetTime(float _t = 0.0f) { playbackTime_ = _t; }

        // --- Getters ---

        /**
         * @brief 現在シミュレーション再生中（再生要求ON）かを判定します。
         */
        bool IsPlaying() const { return isPlaying_; }

        /**
         * @brief 現在シミュレーションが一時停止中かを判定します。
         */
        bool IsPaused() const { return isPaused_; }

        /**
         * @brief 再生経過時間を取得します。
         */
        float GetTime() const { return playbackTime_; }

        // --- Editor Preview ---
        bool isEditorPreview_ = false;
        float editorPlaybackTime_ = 0.0f;
        bool isEditorPaused_ = false;

        // --- Modules ---
        ParticleSystemMain main;
        ParticleSystemEmission emission;
        ParticleSystemShape shape;
        ParticleSystemColorOverLifetime colorOverLifetime;
        ParticleSystemSizeOverLifetime sizeOverLifetime;
        ParticleSystemVelocityOverLifetime velocityOverLifetime;
        ParticleSystemRenderer renderer;

        // --- CPU Simulation State ---
        std::vector<Particle> particles;
        size_t aliveCount = 0;
        float emitAccumulator = 0.0f;
        std::vector<int> burstCycleCounts; // Track how many times a burst has fired

        Matrix4x4 previousWorldMat;
        bool hasPreviousWorldMat = false;

    private:
        bool isPlaying_ = false;
        bool isPaused_ = false;
        float playbackTime_ = 0.0f;

        // GPU related resources will be added here in Phase 4
    };

}
