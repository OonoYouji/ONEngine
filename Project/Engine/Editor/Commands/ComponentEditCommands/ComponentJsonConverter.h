#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Core/Utility/Utility.h"

#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Effect/Effect.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ParticleSystem/ParticleSystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line2DRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line3DRenderer.h"


namespace ONEngine {
/// /////////////////////////////////////////////////////
/// コンポーネントをJSON形式に変換するコマンド
/// /////////////////////////////////////////////////////
namespace ComponentJsonConverter {
nlohmann::json ToJson(const IComponent* _component);
void FromJson(const nlohmann::json& _j, IComponent* _component);
} /// ComponentJsonConverter


/// //////////////////////////////////////////////////
/// utilities
/// //////////////////////////////////////////////////

// quaternion
void from_json(const nlohmann::json& _j, Quaternion& _q);
void to_json(nlohmann::json& _j, const Quaternion& _q);

// color
void from_json(const nlohmann::json& _j, Color& _c);
void to_json(nlohmann::json& _j, const Color& _c);


/// //////////////////////////////////////////////////
/// components
/// //////////////////////////////////////////////////

// DirectionalLight
void from_json(const nlohmann::json& _j, DirectionalLight& _l);
void to_json(nlohmann::json& _j, const DirectionalLight& _l);

// Effect
void from_json(const nlohmann::json& _j, Effect& _e);
void to_json(nlohmann::json& _j, const Effect& _e);
// Effect Member Structs
void from_json(const nlohmann::json& _j, Effect::DistanceEmitData& _e);
void to_json(nlohmann::json& _j, const Effect::DistanceEmitData& _e);
void from_json(const nlohmann::json& _j, Effect::TimeEmitData& _e);
void to_json(nlohmann::json& _j, const Effect::TimeEmitData& _e);
void from_json(const nlohmann::json& _j, EffectMainModule& _e);
void to_json(nlohmann::json& _j, const EffectMainModule& _e);
void from_json(const nlohmann::json& _j, EffectEmitShape& _e);
void to_json(nlohmann::json& _j, const EffectEmitShape& _e);

// CustomMeshRenderer
void from_json(const nlohmann::json& _j, CustomMeshRenderer& _m);
void to_json(nlohmann::json& _j, const CustomMeshRenderer& _m);

// Line2DRenderer
void from_json(const nlohmann::json& _j, Line2DRenderer& _l);
void to_json(nlohmann::json& _j, const Line2DRenderer& _l);

// Line3DRenderer
void from_json(const nlohmann::json& _j, Line3DRenderer& _l);
void to_json(nlohmann::json& _j, const Line3DRenderer& _l);

// ParticleSystem
void from_json(const nlohmann::json& _j, AnimationCurveKey& _k);
void to_json(nlohmann::json& _j, const AnimationCurveKey& _k);
void from_json(const nlohmann::json& _j, AnimationCurve& _c);
void to_json(nlohmann::json& _j, const AnimationCurve& _c);
void from_json(const nlohmann::json& _j, MinMaxCurve& _m);
void to_json(nlohmann::json& _j, const MinMaxCurve& _m);
void from_json(const nlohmann::json& _j, GradientColorKey& _k);
void to_json(nlohmann::json& _j, const GradientColorKey& _k);
void from_json(const nlohmann::json& _j, GradientAlphaKey& _k);
void to_json(nlohmann::json& _j, const GradientAlphaKey& _k);
void from_json(const nlohmann::json& _j, ParticleSystemGradient& _g);
void to_json(nlohmann::json& _j, const ParticleSystemGradient& _g);
void from_json(const nlohmann::json& _j, MinMaxGradient& _m);
void to_json(nlohmann::json& _j, const MinMaxGradient& _m);

void from_json(const nlohmann::json& _j, ParticleSystem& _p);
void to_json(nlohmann::json& _j, const ParticleSystem& _p);
void from_json(const nlohmann::json& _j, MinMaxFloat& _m);
void to_json(nlohmann::json& _j, const MinMaxFloat& _m);
void from_json(const nlohmann::json& _j, MinMaxColor& _m);
void to_json(nlohmann::json& _j, const MinMaxColor& _m);
void from_json(const nlohmann::json& _j, ParticleSystemMain& _m);
void to_json(nlohmann::json& _j, const ParticleSystemMain& _m);
void from_json(const nlohmann::json& _j, ParticleSystemEmission& _e);
void to_json(nlohmann::json& _j, const ParticleSystemEmission& _e);
void from_json(const nlohmann::json& _j, ParticleSystemEmission::Burst& _b);
void to_json(nlohmann::json& _j, const ParticleSystemEmission::Burst& _b);
void from_json(const nlohmann::json& _j, ParticleSystemShape& _s);
void to_json(nlohmann::json& _j, const ParticleSystemShape& _s);
void from_json(const nlohmann::json& _j, ParticleSystemRenderer& _r);
void to_json(nlohmann::json& _j, const ParticleSystemRenderer& _r);
void from_json(const nlohmann::json& _j, ParticleSystemColorOverLifetime& _c);
void to_json(nlohmann::json& _j, const ParticleSystemColorOverLifetime& _c);
void from_json(const nlohmann::json& _j, ParticleSystemSizeOverLifetime& _s);
void to_json(nlohmann::json& _j, const ParticleSystemSizeOverLifetime& _s);
void from_json(const nlohmann::json& _j, ParticleSystemVelocityOverLifetime& _v);
void to_json(nlohmann::json& _j, const ParticleSystemVelocityOverLifetime& _v);


} /// ONEngine
