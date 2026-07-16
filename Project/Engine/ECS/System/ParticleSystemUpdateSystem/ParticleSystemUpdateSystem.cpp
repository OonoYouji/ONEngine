#include "ParticleSystemUpdateSystem.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ParticleSystem/ParticleSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Core/Utility/Time/Time.h"
#include "Engine/Core/Utility/Tools/Random.h"
#include "Engine/Core/Utility/Tools/Gizmo.h"
#include "Engine/Core/Utility/Tools/Log.h"
#include <numbers>
#include <cmath>
#include <algorithm>
#include <chrono>

namespace ONEngine {

    // --- Helper Functions ---

    static float GetMinMaxFloat(const MinMaxFloat& mmf) {
        if (mmf.state == MinMaxState::Constant) return mmf.constant;
        return Random::Float(mmf.minVal, mmf.maxVal);
    }

    static Color GetMinMaxColor(const MinMaxColor& mmc) {
        if (mmc.state == MinMaxState::Constant) return mmc.constant;
        float t = Random::Float(0.0f, 1.0f);
        return Color(
            mmc.minVal.r + (mmc.maxVal.r - mmc.minVal.r) * t,
            mmc.minVal.g + (mmc.maxVal.g - mmc.minVal.g) * t,
            mmc.minVal.b + (mmc.maxVal.b - mmc.minVal.b) * t,
            mmc.minVal.a + (mmc.maxVal.a - mmc.minVal.a) * t
        );
    }

    static float EvaluateMinMaxCurve(const MinMaxCurve& mmc, float time, float randomValue = 0.5f) {
        switch (mmc.state) {
            case MinMaxState::Constant: return mmc.constant;
            case MinMaxState::Curve: return mmc.curve.Evaluate(time);
            case MinMaxState::RandomBetweenTwoCurves: {
                float vMin = mmc.curveMin.Evaluate(time);
                float vMax = mmc.curveMax.Evaluate(time);
                return vMin + (vMax - vMin) * randomValue; 
            }
            default: return mmc.constant;
        }
    }

    static Color EvaluateMinMaxGradient(const MinMaxGradient& mmg, float time, float randomValue = 0.5f) {
        switch (mmg.state) {
            case MinMaxState::Constant: return mmg.gradient.Evaluate(time);
            case MinMaxState::Curve: return mmg.gradient.Evaluate(time);
            case MinMaxState::RandomBetweenTwoCurves: {
                Color cMin = mmg.gradientMin.Evaluate(time);
                Color cMax = mmg.gradientMax.Evaluate(time);
                return Color(
                    cMin.r + (cMax.r - cMin.r) * randomValue,
                    cMin.g + (cMax.g - cMin.g) * randomValue,
                    cMin.b + (cMax.b - cMin.b) * randomValue,
                    cMin.a + (cMax.a - cMin.a) * randomValue
                );
            }
            default: return Color::kWhite;
        }
    }

    static Vector3 Vector3Lerp(const Vector3& a, const Vector3& b, float t) {
        return a + (b - a) * t;
    }

    static void EvaluateShape(const ParticleSystemShape& shape, Vector3& outPos, Vector3& outDir) {
        if (!shape.enabled) {
            outPos = Vector3::Zero;
            outDir = Vector3(0, 0, 1);
            return;
        }

        switch (shape.type) {
            case ParticleSystemShapeType::Sphere: {
                outDir = Random::InsideUnitSphere();
                float lengthSq = outDir.LengthSquared();
                if (lengthSq > 0.0001f) {
                    outDir = outDir / std::sqrt(lengthSq);
                } else {
                    outDir = Vector3(0, 0, 1);
                }
                float t = Random::Float(1.0f - shape.radiusThickness, 1.0f);
                outPos = outDir * (shape.radius * t);
                break;
            }
            case ParticleSystemShapeType::Hemisphere: {
                outDir = Random::InsideUnitSphere();
                outDir.z = std::abs(outDir.z); 
                float lengthSq = outDir.LengthSquared();
                if (lengthSq > 0.0001f) {
                    outDir = outDir / std::sqrt(lengthSq);
                } else {
                    outDir = Vector3(0, 0, 1);
                }
                float t = Random::Float(1.0f - shape.radiusThickness, 1.0f);
                outPos = outDir * (shape.radius * t);
                break;
            }
            case ParticleSystemShapeType::Box: {
                outPos = Vector3(
                    Random::Float(-shape.boxScale.x * 0.5f, shape.boxScale.x * 0.5f),
                    Random::Float(-shape.boxScale.y * 0.5f, shape.boxScale.y * 0.5f),
                    Random::Float(-shape.boxScale.z * 0.5f, shape.boxScale.z * 0.5f)
                );
                outDir = Vector3(0, 0, 1);
                break;
            }
            case ParticleSystemShapeType::Cone: {
                float r = Random::Float(0.0f, 1.0f);
                float theta = Random::Float(0.0f, shape.arc) * 3.14159f / 180.0f;
                float angleRad = shape.angle * 3.14159f / 180.0f;
                float sinAngle = std::sin(angleRad);
                float cosAngle = std::cos(angleRad);
                outDir = Vector3(r * std::cos(theta) * sinAngle, r * std::sin(theta) * sinAngle, cosAngle).Normalize();
                outPos = outDir * (shape.radius * Random::Float(1.0f - shape.radiusThickness, 1.0f));
                break;
            }
            case ParticleSystemShapeType::Circle: {
                float theta = Random::Float(0.0f, shape.arc) * 3.14159f / 180.0f;
                float r = std::sqrt(Random::Float(0.0f, 1.0f)); 
                float t = Random::Float(1.0f - shape.radiusThickness, 1.0f);
                r *= t * shape.radius;
                outPos = Vector3(r * std::cos(theta), r * std::sin(theta), 0.0f);
                outDir = Vector3(0, 0, 1); 
                break;
            }
            case ParticleSystemShapeType::Edge: {
                outPos = Vector3(Random::Float(-shape.radius, shape.radius), 0, 0);
                outDir = Vector3(0, 0, 1);
                break;
            }
            default:
                outPos = Vector3::Zero;
                outDir = Vector3(0, 0, 1);
                break;
        }
    }

    // --- System Methods ---

    /**
     * @brief コンストラクタ
     */
    ParticleSystemUpdateSystem::ParticleSystemUpdateSystem() : ECSISystem() {}

    /**
     * @brief エディタ非実行時のパーティクル更新処理（およびギズモ等の表示）を行います。
     */
    void ParticleSystemUpdateSystem::OutsideOfRuntimeUpdate(class ECSGroup* _ecs) {
        DrawGizmos(_ecs);

        // --- Editor Preview Update ---
        // エンジンのUnscaledDeltaTimeを使用して実時間でのシミュレーションを行う
        float dt = Time::UnscaledDeltaTime();

        // 停止中や不安定な場合の補正
        if (dt <= 0.0f || dt > 0.1f) dt = 1.0f / 60.0f;

        auto& entities = _ecs->GetEntities();
        for (auto& entityPtr : entities) {
            GameEntity* entity = entityPtr.get();
            if (!entity || !entity->active) continue;

            auto* ps = entity->GetComponent<ParticleSystem>();
            if (!ps || !ps->enable || !ps->isEditorPreview_ || ps->isEditorPaused_) continue;

            UpdateSingleSystem(ps, entity, dt);
        }
    }

    /**
     * @brief 毎フレームのパーティクルシミュレーション状態の更新処理を実行します。
     */
    void ParticleSystemUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {
        DrawGizmos(_ecs);

        float dt = Time::DeltaTime();
        if (dt <= 0.0f) return;

        auto& entities = _ecs->GetEntities();
        for (auto& entityPtr : entities) {
            GameEntity* entity = entityPtr.get();
            if (!entity || !entity->active) continue;

            auto* ps = entity->GetComponent<ParticleSystem>();
            if (!ps || !ps->enable) continue;

            UpdateSingleSystem(ps, entity, dt);
        }
    }

    /**
     * @brief 単一の ParticleSystem コンポーネントにおけるシミュレーション計算を1ステップ進めます。
     */
    void ParticleSystemUpdateSystem::UpdateSingleSystem(ParticleSystem* ps, GameEntity* entity, float dt) {
        Transform* transform = entity->GetTransform();
        if (!transform) return;

        if (ps->main.playOnAwake && ps->GetTime() == 0.0f && !ps->IsPlaying() && !ps->IsPaused()) {
            ps->Play();
        }

        if (ps->IsPlaying() && !ps->IsPaused()) {
            ps->UpdateTime(dt);
        }

        bool isSimulating = ps->IsPlaying() && !ps->IsPaused();

        if (ps->particles.size() != static_cast<size_t>(ps->main.maxParticles)) {
            ps->particles.resize(ps->main.maxParticles);
            if (ps->aliveCount > ps->particles.size()) ps->aliveCount = ps->particles.size();
        }

        if (ps->burstCycleCounts.size() != ps->emission.bursts.size()) {
            ps->burstCycleCounts.resize(ps->emission.bursts.size(), 0);
        }

        float currentPlaybackTime = ps->GetTime();

        // --- Emission ---
        if (isSimulating && ps->emission.enabled && currentPlaybackTime > GetMinMaxFloat(ps->main.startDelay)) {
            ps->emitAccumulator += ps->emission.rateOverTime * dt;
            int emitCount = static_cast<int>(ps->emitAccumulator);
            ps->emitAccumulator -= static_cast<float>(emitCount);

            for (int i = 0; i < emitCount; ++i) {
                if (ps->aliveCount >= ps->particles.size()) break;

                Particle& p = ps->particles[ps->aliveCount++];
                Vector3 shapePos, shapeDir;
                EvaluateShape(ps->shape, shapePos, shapeDir);

                Matrix4x4 currentMat = transform->matWorld;
                Matrix4x4 emitMat = currentMat;
                if (ps->hasPreviousWorldMat) {
                    float t_emit = (float)i / (float)(emitCount > 1 ? emitCount - 1 : 1);
                    Vector3 prevPos(ps->previousWorldMat.m[3][0], ps->previousWorldMat.m[3][1], ps->previousWorldMat.m[3][2]);
                    Vector3 currPos(currentMat.m[3][0], currentMat.m[3][1], currentMat.m[3][2]);
                    Vector3 lerpedPos = Vector3Lerp(prevPos, currPos, t_emit);
                    emitMat.m[3][0] = lerpedPos.x;
                    emitMat.m[3][1] = lerpedPos.y;
                    emitMat.m[3][2] = lerpedPos.z;
                }

                if (ps->main.simulationSpace == SimulationSpace::World) {
                    p.position = Matrix4x4::Transform(shapePos, emitMat);
                    p.velocity = Matrix4x4::TransformNormal(shapeDir, emitMat).Normalize() * GetMinMaxFloat(ps->main.startSpeed);
                    p.simulationSpace = 0; // World
                } else {
                    p.position = shapePos;
                    p.velocity = shapeDir * GetMinMaxFloat(ps->main.startSpeed);
                    p.simulationSpace = 1; // Local
                }

                p.baseVelocity = p.velocity;
                p.startLifetime = GetMinMaxFloat(ps->main.startLifetime);
                p.remainingLifetime = p.startLifetime;
                p.startColor = GetMinMaxColor(ps->main.startColor);
                p.color = p.startColor;
                p.startSize = GetMinMaxFloat(ps->main.startSize);
                p.size = p.startSize;
                p.rotation = GetMinMaxFloat(ps->main.startRotation);
                p.randomValue = Random::Float(0.0f, 1.0f);
            }

            for (size_t i = 0; i < ps->emission.bursts.size(); ++i) {
                auto& burst = ps->emission.bursts[i];
                int& cycleCount = ps->burstCycleCounts[i];
                
                float burstTime = burst.time;
                if (currentPlaybackTime >= burstTime && cycleCount < burst.cycles) {
                    float nextBurstTime = burstTime + static_cast<float>(cycleCount) * burst.interval;
                    if (currentPlaybackTime >= nextBurstTime) {
                        if (Random::Float(0.0f, 1.0f) <= burst.probability) {
                            int burstEmitCount = burst.count;
                            for (int e = 0; e < burstEmitCount; ++e) {
                                if (ps->aliveCount >= ps->particles.size()) break;
                                Particle& p = ps->particles[ps->aliveCount++];
                                Vector3 shapePos, shapeDir;
                                EvaluateShape(ps->shape, shapePos, shapeDir);
                                
                                Matrix4x4 worldMat = transform->matWorld;
                                if (ps->main.simulationSpace == SimulationSpace::World) {
                                    p.position = Matrix4x4::Transform(shapePos, worldMat);
                                    p.velocity = Matrix4x4::TransformNormal(shapeDir, worldMat).Normalize() * GetMinMaxFloat(ps->main.startSpeed);
                                    p.simulationSpace = 0; // World
                                } else {
                                    p.position = shapePos;
                                    p.velocity = shapeDir * GetMinMaxFloat(ps->main.startSpeed);
                                    p.simulationSpace = 1; // Local
                                }

                                p.startLifetime = GetMinMaxFloat(ps->main.startLifetime);
                                p.remainingLifetime = p.startLifetime;
                                p.startColor = GetMinMaxColor(ps->main.startColor);
                                p.color = p.startColor;
                                p.startSize = GetMinMaxFloat(ps->main.startSize);
                                p.size = p.startSize;
                                p.rotation = GetMinMaxFloat(ps->main.startRotation);
                                p.randomValue = Random::Float(0.0f, 1.0f);
                            }
                        }
                        cycleCount++;
                    }
                }
            }
        }

        if (isSimulating && currentPlaybackTime > ps->main.duration) {
            if (ps->main.looping) {
                ps->ResetTime(currentPlaybackTime - ps->main.duration);
                std::fill(ps->burstCycleCounts.begin(), ps->burstCycleCounts.end(), 0);
            } else {
                ps->Stop();
            }
        }

        // --- Update ---
        if (!ps->IsPaused() && ps->IsPlaying()) {
            Vector3 gravity = Vector3(0.0f, -9.81f, 0.0f) * ps->main.gravityModifier;
            
            for (size_t i = 0; i < ps->aliveCount; ) {
                Particle& p = ps->particles[i];
                p.remainingLifetime -= dt;

                if (p.remainingLifetime <= 0.0f) {
                    if (ps->aliveCount > 1) {
                        p = ps->particles[ps->aliveCount - 1];
                    }
                    ps->aliveCount--;
                } else {
                    float normalizedTime = 1.0f - (p.remainingLifetime / p.startLifetime);
                    normalizedTime = std::clamp(normalizedTime, 0.0f, 1.0f);

                    if (ps->velocityOverLifetime.enabled) {
                        Vector3 linearVelocity(
                            EvaluateMinMaxCurve(ps->velocityOverLifetime.x, normalizedTime, p.randomValue),
                            EvaluateMinMaxCurve(ps->velocityOverLifetime.y, normalizedTime, p.randomValue),
                            EvaluateMinMaxCurve(ps->velocityOverLifetime.z, normalizedTime, p.randomValue)
                        );
                        float speedMultiplier = EvaluateMinMaxCurve(ps->velocityOverLifetime.speedModifier, normalizedTime, p.randomValue);

                        if (ps->velocityOverLifetime.space == SimulationSpace::World) {
                            p.velocity = p.baseVelocity * speedMultiplier + linearVelocity;
                        } else {
                            p.velocity = p.baseVelocity * speedMultiplier + Matrix4x4::TransformNormal(linearVelocity, transform->matWorld);
                        }
                    }

                    // 重力の適用 (Local Simulationの場合は重力もローカル座標系に変換する必要があるが、一旦簡易化してWorldのみか、Localの場合は無視するか検討)
                    // Unity ShurikenのLocal Spaceでは重力はWorld空間のまま掛かるが、パーティクルの速度には適切に反映される。
                    if (p.simulationSpace == 1) { // Local
                        // ローカル座標系での重力方向を計算
                        Vector3 localGravity = Matrix4x4::TransformNormal(gravity, Matrix4x4::MakeInverse(transform->matWorld));
                        p.velocity += localGravity * dt;
                    } else {
                        p.velocity += gravity * dt;
                    }

                    p.position += p.velocity * dt;

                    if (ps->colorOverLifetime.enabled) {
                        Color overLifeColor = EvaluateMinMaxGradient(ps->colorOverLifetime.color, normalizedTime, p.randomValue);
                        p.color.r = p.startColor.r * overLifeColor.r;
                        p.color.g = p.startColor.g * overLifeColor.g;
                        p.color.b = p.startColor.b * overLifeColor.b;
                        p.color.a = p.startColor.a * overLifeColor.a;
                    }
                    if (ps->sizeOverLifetime.enabled) {
                        p.size = p.startSize * EvaluateMinMaxCurve(ps->sizeOverLifetime.size, normalizedTime, p.randomValue);
                    }
                    i++;
                }
            }
        }
        ps->previousWorldMat = transform->matWorld;
        ps->hasPreviousWorldMat = true;
    }

    /**
     * @brief エディタ用：パーティクルエミッター（形状等）のデバッグ用ギズモを描画します。
     */
    void ParticleSystemUpdateSystem::DrawGizmos(class ECSGroup* _ecs) {
        if (!_ecs) return;
        auto& entities = _ecs->GetEntities();
        for (auto& entityPtr : entities) {
            GameEntity* entity = entityPtr.get();
            if (!entity || !entity->active) continue;

            auto* ps = entity->GetComponent<ParticleSystem>();
            if (!ps || !ps->enable) continue;

            auto* transform = entity->GetTransform();
            if (!transform) continue;

            const auto& shape = ps->shape;
            if (!shape.enabled) continue;

            Matrix4x4 worldMat = transform->matWorld;
            Vector4 color = Vector4(1.0f, 1.0f, 0.0f, 0.5f);

            float outerRadius = shape.radius;
            float innerRadius = shape.radius * (1.0f - shape.radiusThickness);

            auto DrawCircle = [&](const Matrix4x4& m, float r, float arc, int axis, float z = 0.0f) {
                if (r <= 0.0f) return;
                const int segments = 32;
                float arcRad = arc * 3.14159f / 180.0f;
                for (int i = 0; i < segments; i++) {
                    float t1 = (float)i / segments * arcRad;
                    float t2 = (float)(i + 1) / segments * arcRad;
                    Vector3 p1, p2;
                    if (axis == 0) { p1 = { r * std::cos(t1), r * std::sin(t1), z }; p2 = { r * std::cos(t2), r * std::sin(t2), z }; }
                    else if (axis == 1) { p1 = { r * std::cos(t1), z, r * std::sin(t1) }; p2 = { r * std::cos(t2), z, r * std::sin(t2) }; }
                    else { p1 = { z, r * std::cos(t1), r * std::sin(t1) }; p2 = { z, r * std::cos(t2), r * std::sin(t2) }; }
                    Gizmo::DrawLine(Matrix4x4::Transform(p1, m), Matrix4x4::Transform(p2, m), color);
                }
            };

            switch (shape.type) {
                case ParticleSystemShapeType::Sphere:
                    DrawCircle(worldMat, outerRadius, 360, 0);
                    DrawCircle(worldMat, outerRadius, 360, 1);
                    DrawCircle(worldMat, outerRadius, 360, 2);
                    if (shape.radiusThickness < 1.0f) {
                        DrawCircle(worldMat, innerRadius, 360, 0);
                        DrawCircle(worldMat, innerRadius, 360, 1);
                        DrawCircle(worldMat, innerRadius, 360, 2);
                    }
                    break;
                case ParticleSystemShapeType::Hemisphere:
                {
                    const int segments = 32;
                    for (int i = 0; i < segments; i++) {
                        float t1 = (float)i / segments * 3.14159f;
                        float t2 = (float)(i + 1) / segments * 3.14159f;
                        // XZ half circle (+Z direction)
                        Gizmo::DrawLine(Matrix4x4::Transform({ outerRadius * std::cos(t1), 0, outerRadius * std::sin(t1) }, worldMat),
                                        Matrix4x4::Transform({ outerRadius * std::cos(t2), 0, outerRadius * std::sin(t2) }, worldMat), color);
                        // YZ half circle (+Z direction)
                        Gizmo::DrawLine(Matrix4x4::Transform({ 0, outerRadius * std::cos(t1), outerRadius * std::sin(t1) }, worldMat),
                                        Matrix4x4::Transform({ 0, outerRadius * std::cos(t2), outerRadius * std::sin(t2) }, worldMat), color);
                    }
                    DrawCircle(worldMat, outerRadius, 360, 0);
                    if (shape.radiusThickness < 1.0f) {
                        for (int i = 0; i < segments; i++) {
                            float t1 = (float)i / segments * 3.14159f;
                            float t2 = (float)(i + 1) / segments * 3.14159f;
                            Gizmo::DrawLine(Matrix4x4::Transform({ innerRadius * std::cos(t1), 0, innerRadius * std::sin(t1) }, worldMat),
                                            Matrix4x4::Transform({ innerRadius * std::cos(t2), 0, innerRadius * std::sin(t2) }, worldMat), color);
                            Gizmo::DrawLine(Matrix4x4::Transform({ 0, innerRadius * std::cos(t1), innerRadius * std::sin(t1) }, worldMat),
                                            Matrix4x4::Transform({ 0, innerRadius * std::cos(t2), innerRadius * std::sin(t2) }, worldMat), color);
                        }
                        DrawCircle(worldMat, innerRadius, 360, 0);
                    }
                }
                break;
                case ParticleSystemShapeType::Box:
                {
                    Vector3 h = shape.boxScale * 0.5f;
                    Vector3 v[8] = {
                        {-h.x, -h.y, -h.z}, {h.x, -h.y, -h.z}, {h.x, h.y, -h.z}, {-h.x, h.y, -h.z},
                        {-h.x, -h.y, h.z}, {h.x, -h.y, h.z}, {h.x, h.y, h.z}, {-h.x, h.y, h.z}
                    };
                    for (int i = 0; i < 8; i++) v[i] = Matrix4x4::Transform(v[i], worldMat);
                    for (int i = 0; i < 4; i++) {
                        Gizmo::DrawLine(v[i], v[(i + 1) % 4], color);
                        Gizmo::DrawLine(v[i + 4], v[((i + 1) % 4) + 4], color);
                        Gizmo::DrawLine(v[i], v[i + 4], color);
                    }
                }
                break;
                case ParticleSystemShapeType::Cone:
                {
                    float angleRad = shape.angle * 3.14159f / 180.0f;
                    float topOuterR = outerRadius + std::tan(angleRad); 
                    float topInnerR = innerRadius + std::tan(angleRad);
                    if (topInnerR < 0.0f) topInnerR = 0.0f;

                    // 外側の円錐
                    DrawCircle(worldMat, outerRadius, shape.arc, 0);
                    DrawCircle(worldMat, topOuterR, shape.arc, 0, 1.0f); // 高さ1.0の位置
                    
                    // 内側の円錐 (厚みが設定されている場合)
                    if (shape.radiusThickness < 1.0f) {
                        DrawCircle(worldMat, innerRadius, shape.arc, 0);
                        DrawCircle(worldMat, topInnerR, shape.arc, 0, 1.0f);
                    }
                    
                    int lines = 8;
                    float arcRad = shape.arc * 3.14159f / 180.0f;
                    for (int i = 0; i <= lines; i++) {
                        float t = (float)i / lines * arcRad;
                        Vector3 pBaseOut = { outerRadius * std::cos(t), outerRadius * std::sin(t), 0 };
                        Vector3 pTopOut = { topOuterR * std::cos(t), topOuterR * std::sin(t), 1.0f };
                        Gizmo::DrawLine(Matrix4x4::Transform(pBaseOut, worldMat), Matrix4x4::Transform(pTopOut, worldMat), color);

                        if (shape.radiusThickness < 1.0f) {
                            Vector3 pBaseIn = { innerRadius * std::cos(t), innerRadius * std::sin(t), 0 };
                            Vector3 pTopIn = { topInnerR * std::cos(t), topInnerR * std::sin(t), 1.0f };
                            Gizmo::DrawLine(Matrix4x4::Transform(pBaseIn, worldMat), Matrix4x4::Transform(pTopIn, worldMat), color);
                        }
                    }
                }
                break;
                case ParticleSystemShapeType::Circle:
                    DrawCircle(worldMat, outerRadius, shape.arc, 0);
                    if (shape.radiusThickness < 1.0f) DrawCircle(worldMat, innerRadius, shape.arc, 0);
                    if (shape.arc < 360.0f) {
                        float a = shape.arc * 3.14159f / 180.0f;
                        float rMin = (shape.radiusThickness >= 1.0f) ? 0.0f : innerRadius;
                        Gizmo::DrawLine(Matrix4x4::Transform({ rMin, 0, 0 }, worldMat), Matrix4x4::Transform({ outerRadius, 0, 0 }, worldMat), color);
                        Gizmo::DrawLine(Matrix4x4::Transform({ rMin * std::cos(a), rMin * std::sin(a), 0 }, worldMat), 
                                        Matrix4x4::Transform({ outerRadius * std::cos(a), outerRadius * std::sin(a), 0 }, worldMat), color);
                    }
                    break;
                case ParticleSystemShapeType::Edge:
                    Gizmo::DrawLine(Matrix4x4::Transform({ -shape.radius, 0, 0 }, worldMat), Matrix4x4::Transform({ shape.radius, 0, 0 }, worldMat), color);
                    break;
            }
        }
    }
}
