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

    class ParticleSystem : public IComponent {
    public:
        ParticleSystem();
        ~ParticleSystem() override = default;

        // --- Controls ---
        void Play();
        void Stop();
        void Clear();
        void Pause();

        void UpdateTime(float _dt) { playbackTime_ += _dt; }
        void ResetTime(float _t = 0.0f) { playbackTime_ = _t; }

        // --- Getters ---
        bool IsPlaying() const { return isPlaying_; }
        bool IsPaused() const { return isPaused_; }
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
