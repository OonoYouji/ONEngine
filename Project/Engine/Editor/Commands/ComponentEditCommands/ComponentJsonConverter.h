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
namespace ComponentJsonConverter {
    /**
     * @brief 任意のIComponentポインタから、型情報を判別し対応するJSONオブジェクトへシリアライズ（データエクスポート）します。
     * @param _component 変換対象のコンポーネントへのポインタ
     * @return シリアライズされたJSONオブジェクト
     */
	nlohmann::json ToJson(const IComponent* _component);

    /**
     * @brief JSONオブジェクトからデータをパースし、対応するコンポーネントインスタンスへパラメータを割り当て復元（デシリアライズ）します。
     * @param _j 読み込むJSONオブジェクト
     * @param _component 復元先のコンポーネントインスタンスへのポインタ
     */
	void FromJson(const nlohmann::json& _j, IComponent* _component);
} /// ComponentJsonConverter


/// //////////////////////////////////////////////////
/// utilities (シリアライザー)
/// //////////////////////////////////////////////////

/**
 * @brief Quaternion デシリアライズ
 */
void from_json(const nlohmann::json& _j, Quaternion& _q);

/**
 * @brief Quaternion シリアライズ
 */
void to_json(nlohmann::json& _j, const Quaternion& _q);

/**
 * @brief Color デシリアライズ
 */
void from_json(const nlohmann::json& _j, Color& _c);

/**
 * @brief Color シリアライズ
 */
void to_json(nlohmann::json& _j, const Color& _c);


/// //////////////////////////////////////////////////
/// components (シリアライザー)
/// //////////////////////////////////////////////////

/**
 * @brief DirectionalLight デシリアライズ
 */
void from_json(const nlohmann::json& _j, DirectionalLight& _l);

/**
 * @brief DirectionalLight シリアライズ
 */
void to_json(nlohmann::json& _j, const DirectionalLight& _l);

/**
 * @brief Effect デシリアライズ
 */
void from_json(const nlohmann::json& _j, Effect& _e);

/**
 * @brief Effect シリアライズ
 */
void to_json(nlohmann::json& _j, const Effect& _e);

/**
 * @brief Effect::DistanceEmitData デシリアライズ
 */
void from_json(const nlohmann::json& _j, Effect::DistanceEmitData& _e);

/**
 * @brief Effect::DistanceEmitData シリアライズ
 */
void to_json(nlohmann::json& _j, const Effect::DistanceEmitData& _e);

/**
 * @brief Effect::TimeEmitData デシリアライズ
 */
void from_json(const nlohmann::json& _j, Effect::TimeEmitData& _e);

/**
 * @brief Effect::TimeEmitData シリアライズ
 */
void to_json(nlohmann::json& _j, const Effect::TimeEmitData& _e);

/**
 * @brief EffectMainModule デシリアライズ
 */
void from_json(const nlohmann::json& _j, EffectMainModule& _e);

/**
 * @brief EffectMainModule シリアライズ
 */
void to_json(nlohmann::json& _j, const EffectMainModule& _e);

/**
 * @brief EffectEmitShape デシリアライズ
 */
void from_json(const nlohmann::json& _j, EffectEmitShape& _e);

/**
 * @brief EffectEmitShape シリアライズ
 */
void to_json(nlohmann::json& _j, const EffectEmitShape& _e);

/**
 * @brief CustomMeshRenderer デシリアライズ
 */
void from_json(const nlohmann::json& _j, CustomMeshRenderer& _m);

/**
 * @brief CustomMeshRenderer シリアライズ
 */
void to_json(nlohmann::json& _j, const CustomMeshRenderer& _m);

/**
 * @brief Line2DRenderer デシリアライズ
 */
void from_json(const nlohmann::json& _j, Line2DRenderer& _l);

/**
 * @brief Line2DRenderer シリアライズ
 */
void to_json(nlohmann::json& _j, const Line2DRenderer& _l);

/**
 * @brief Line3DRenderer デシリアライズ
 */
void from_json(const nlohmann::json& _j, Line3DRenderer& _l);

/**
 * @brief Line3DRenderer シリアライズ
 */
void to_json(nlohmann::json& _j, const Line3DRenderer& _l);

/**
 * @brief AnimationCurveKey デシリアライズ
 */
void from_json(const nlohmann::json& _j, AnimationCurveKey& _k);

/**
 * @brief AnimationCurveKey シリアライズ
 */
void to_json(nlohmann::json& _j, const AnimationCurveKey& _k);

/**
 * @brief AnimationCurve デシリアライズ
 */
void from_json(const nlohmann::json& _j, AnimationCurve& _c);

/**
 * @brief AnimationCurve シリアライズ
 */
void to_json(nlohmann::json& _j, const AnimationCurve& _c);

/**
 * @brief MinMaxCurve デシリアライズ
 */
void from_json(const nlohmann::json& _j, MinMaxCurve& _m);

/**
 * @brief MinMaxCurve シリアライズ
 */
void to_json(nlohmann::json& _j, const MinMaxCurve& _m);

/**
 * @brief GradientColorKey デシリアライズ
 */
void from_json(const nlohmann::json& _j, GradientColorKey& _k);

/**
 * @brief GradientColorKey シリアライズ
 */
void to_json(nlohmann::json& _j, const GradientColorKey& _k);

/**
 * @brief GradientAlphaKey デシリアライズ
 */
void from_json(const nlohmann::json& _j, GradientAlphaKey& _k);

/**
 * @brief GradientAlphaKey シリアライズ
 */
void to_json(nlohmann::json& _j, const GradientAlphaKey& _k);

/**
 * @brief ParticleSystemGradient デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystemGradient& _g);

/**
 * @brief ParticleSystemGradient シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystemGradient& _g);

/**
 * @brief MinMaxGradient デシリアライズ
 */
void from_json(const nlohmann::json& _j, MinMaxGradient& _m);

/**
 * @brief MinMaxGradient シリアライズ
 */
void to_json(nlohmann::json& _j, const MinMaxGradient& _m);

/**
 * @brief ParticleSystem デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystem& _p);

/**
 * @brief ParticleSystem シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystem& _p);

/**
 * @brief MinMaxFloat デシリアライズ
 */
void from_json(const nlohmann::json& _j, MinMaxFloat& _m);

/**
 * @brief MinMaxFloat シリアライズ
 */
void to_json(nlohmann::json& _j, const MinMaxFloat& _m);

/**
 * @brief MinMaxColor デシリアライズ
 */
void from_json(const nlohmann::json& _j, MinMaxColor& _m);

/**
 * @brief MinMaxColor シリアライズ
 */
void to_json(nlohmann::json& _j, const MinMaxColor& _m);

/**
 * @brief ParticleSystemMain デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystemMain& _m);

/**
 * @brief ParticleSystemMain シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystemMain& _m);

/**
 * @brief ParticleSystemEmission デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystemEmission& _e);

/**
 * @brief ParticleSystemEmission シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystemEmission& _e);

/**
 * @brief ParticleSystemEmission::Burst デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystemEmission::Burst& _b);

/**
 * @brief ParticleSystemEmission::Burst シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystemEmission::Burst& _b);

/**
 * @brief ParticleSystemShape デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystemShape& _s);

/**
 * @brief ParticleSystemShape シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystemShape& _s);

/**
 * @brief ParticleSystemRenderer デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystemRenderer& _r);

/**
 * @brief ParticleSystemRenderer シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystemRenderer& _r);

/**
 * @brief ParticleSystemColorOverLifetime デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystemColorOverLifetime& _c);

/**
 * @brief ParticleSystemColorOverLifetime シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystemColorOverLifetime& _c);

/**
 * @brief ParticleSystemSizeOverLifetime デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystemSizeOverLifetime& _s);

/**
 * @brief ParticleSystemSizeOverLifetime シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystemSizeOverLifetime& _s);

/**
 * @brief ParticleSystemVelocityOverLifetime デシリアライズ
 */
void from_json(const nlohmann::json& _j, ParticleSystemVelocityOverLifetime& _v);

/**
 * @brief ParticleSystemVelocityOverLifetime シリアライズ
 */
void to_json(nlohmann::json& _j, const ParticleSystemVelocityOverLifetime& _v);


} /// ONEngine
