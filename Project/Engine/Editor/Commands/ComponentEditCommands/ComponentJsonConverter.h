#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Engine/ECS/Component/Component.h"
#include "Engine/Core/Utility/Utility.h"



/// /////////////////////////////////////////////////////
/// コンポーネントをJSON形式に変換するコマンド
/// /////////////////////////////////////////////////////
namespace ComponentJsonConverter {
	nlohmann::json ToJson(const IComponent* _component);
};



/// //////////////////////////////////////////////////
/// utilities
/// //////////////////////////////////////////////////

// vector2
void from_json(const nlohmann::json& _j, Vec2& _v);
void to_json(nlohmann::json& _j, const Vec2& _v);

// vector3
void from_json(const nlohmann::json& _j, Vec3& _v);
void to_json(nlohmann::json& _j, const Vec3& _v);

// vector4
void from_json(const nlohmann::json& _j, Vec4& _v);
void to_json(nlohmann::json& _j, const Vec4& _v);

// quaternion
void from_json(const nlohmann::json& _j, Quaternion& _q);
void to_json(nlohmann::json& _j, const Quaternion& _q);

// color
void from_json(const nlohmann::json& _j, Color& _c);
void to_json(nlohmann::json& _j, const Color& _c);


/// //////////////////////////////////////////////////
/// components
/// //////////////////////////////////////////////////

// Transform
void from_json(const nlohmann::json& _j, Transform& _t);
void to_json(nlohmann::json& _j, const Transform& _t);

// DirectionalLight
void from_json(const nlohmann::json& _j, DirectionalLight& _l);
void to_json(nlohmann::json& _j, const DirectionalLight& _l);

// AudioSource
void from_json(const nlohmann::json& _j, AudioSource& _a);
void to_json(nlohmann::json& _j, const AudioSource& _a);

// Variables
void from_json(const nlohmann::json& _j, Variables& _v);
void to_json(nlohmann::json& _j, const Variables& _v);

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
// エフェクトのエミッターの形状
void from_json(const nlohmann::json& _j, EffectEmitShape::Sphere& _e);
void to_json(nlohmann::json& _j, const EffectEmitShape::Sphere& _e);
void from_json(const nlohmann::json& _j, EffectEmitShape::Cube& _e);
void to_json(nlohmann::json& _j, const EffectEmitShape::Cube& _e);
void from_json(const nlohmann::json& _j, EffectEmitShape::Cone& _e);
void to_json(nlohmann::json& _j, const EffectEmitShape::Cone& _e);

// MeshRenderer
void from_json(const nlohmann::json& _j, MeshRenderer& _m);
void to_json(nlohmann::json& _j, const MeshRenderer& _m);

// CustomMeshRenderer
void from_json(const nlohmann::json& _j, CustomMeshRenderer& _m);
void to_json(nlohmann::json& _j, const CustomMeshRenderer& _m);

// SpriteRenderer
void from_json(const nlohmann::json& _j, SpriteRenderer& _s);
void to_json(nlohmann::json& _j, const SpriteRenderer& _s);

// Line2DRenderer
void from_json(const nlohmann::json& _j, Line2DRenderer& _l);
void to_json(nlohmann::json& _j, const Line2DRenderer& _l);

// Line3DRenderer
void from_json(const nlohmann::json& _j, Line3DRenderer& _l);
void to_json(nlohmann::json& _j, const Line3DRenderer& _l);

// ToTerrainCollider
void from_json(const nlohmann::json& _j, ToTerrainCollider& _c);
void to_json(nlohmann::json& _j, const ToTerrainCollider& _c);
