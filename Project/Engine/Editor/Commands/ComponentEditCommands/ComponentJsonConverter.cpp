#include "ComponentJsonConverter.h"

/// std
#include <unordered_map>
#include <typeindex>
#include <functional>

#include "Engine/ECS/Component/Collection/ComponentHash.h"

/// engine/compute
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Audio/AudioSource.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ParticleSystem/ParticleSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Variables/Variables.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Agent/AgentIntentComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animator/Animator.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animation/AnimationPlayer.h"

/// engine/renderer
#include "Engine/ECS/Component/Components/RendererComponents/Skybox/Skybox.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/DissolveMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/ScreenPostEffectTag/ScreenPostEffectTag.h"


using namespace ONEngine;


/// //////////////////////////////////////////////////
/// ComponentJsonConverter
/// //////////////////////////////////////////////////

namespace {
	class JsonConverter {
	public:

		JsonConverter() {

			/// compute
			Register<Transform>();
			Register<Variables>();
			Register<DirectionalLight>();
			Register<AudioSource>();
			Register<Effect>();
			Register<ParticleSystem>();
			Register<Script>();
			Register<Terrain>();
			Register<GrassField>();
			Register<TerrainCollider>();
			Register<CameraComponent>();
			Register<ShadowCaster>();
			Register<VoxelTerrain>();
			Register<AgentIntentComponent>();
			Register<Animator>();
			Register<AnimationPlayer>();

			/// renderer
			Register<SpriteRenderer>();
			Register<CustomMeshRenderer>();
			Register<MeshRenderer>();
			Register<DissolveMeshRenderer>();
			Register<SkinMeshRenderer>();
			Register<Line2DRenderer>();
			Register<Line3DRenderer>();
			Register<ScreenPostEffectTag>();
			Register<Skybox>();

			/// collision
			Register<SphereCollider>();
			Register<BoxCollider>();
		}

		template <typename T>
		void Register() {
			std::string typeName = GetComponentTypeName<T>();

			converters_[typeName] = [](const IComponent* _component) {
				return *static_cast<const T*>(_component);
				};

			fromJsonConverters_[typeName] = [this](IComponent* _component, const nlohmann::json& _j) {
				uint32_t id = _component->id;
 				*static_cast<T*>(_component) = _j.get<T>();
				_component->id = id;
				};
		}

		using Converter = std::function<nlohmann::json(const IComponent*)>;
		using FromJsonConverter = std::function<void(IComponent*, const nlohmann::json&)>;
		std::unordered_map<std::string, Converter> converters_;
		std::unordered_map<std::string, FromJsonConverter> fromJsonConverters_;

	};

	JsonConverter jsonConverter;
}

nlohmann::json ComponentJsonConverter::ToJson(const IComponent* _component) {
	std::string name = GetComponentTypeName(_component);

	auto itr = jsonConverter.converters_.find(name);
	if (itr == jsonConverter.converters_.end()) {
		return nlohmann::json{};
	}

	return itr->second(_component);
}

void ComponentJsonConverter::FromJson(const nlohmann::json& _j, IComponent* _component) {
	std::string name = GetComponentTypeName(_component);

	auto itr = jsonConverter.fromJsonConverters_.find(name);
	if (itr == jsonConverter.fromJsonConverters_.end()) {
		Console::Log("ComponentJsonConverter: " + name + "の変換関数が登録されていません。");
		return;
	}

	itr->second(_component, _j);
}

void ONEngine::from_json(const nlohmann::json& _j, Quaternion& _q) {
	_q.x = _j.at("x").get<float>();
	_q.y = _j.at("y").get<float>();
	_q.z = _j.at("z").get<float>();
	_q.w = _j.at("w").get<float>();
}

void ONEngine::to_json(nlohmann::json& _j, const Quaternion& _q) {
	_j = nlohmann::json{ { "x", _q.x }, { "y", _q.y }, { "z", _q.z }, { "w", _q.w } };
}

void ONEngine::from_json(const nlohmann::json& _j, Color& _c) {
	_c.r = _j.at("r").get<float>();
	_c.g = _j.at("g").get<float>();
	_c.b = _j.at("b").get<float>();
	_c.a = _j.at("a").get<float>();
}

void ONEngine::to_json(nlohmann::json& _j, const Color& _c) {
	_j = nlohmann::json{ { "r", _c.r }, { "g", _c.g }, { "b", _c.b }, { "a", _c.a } };
}


void ONEngine::from_json(const nlohmann::json& _j, DirectionalLight& _l) {
	_l.SetIntensity(_j.at("intensity").get<float>());
	_l.SetDirection(_j.at("direction").get<Vector3>());
	_l.SetColor(_j.at("color").get<Vector4>());
}

void ONEngine::to_json(nlohmann::json& _j, const DirectionalLight& _l) {
	_j = nlohmann::json{
		{ "type", "DirectionalLight" },
		{ "enable", _l.enable },
		{ "intensity", _l.GetIntensity() },
		{ "direction", _l.GetDirection() },
		{ "color", _l.GetColor() }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, Effect& _e) {
	_e.enable = _j.at("enable").get<int>();
	_e.SetIsCreateParticle(_j.at("isCreateParticle").get<bool>());
	_e.SetMeshPath(_j.at("meshPath").get<std::string>());
	_e.SetTexturePath(_j.at("texturePath").get<std::string>());
	_e.SetMainModule(_j.at("mainModule").get<EffectMainModule>());
	_e.SetEmitShape(_j.at("emitShape").get<EffectEmitShape>());
	_e.SetEmitType(static_cast<Effect::EmitType>(_j.at("emitType").get<int>()));
	_e.SetEmitTypeDistance(_j.at("distanceEmitData").get<Effect::DistanceEmitData>());
	_e.SetEmitTypeTime(_j.at("timeEmitData").get<Effect::TimeEmitData>());
	_e.SetMaxEffectCount(_j.at("maxEffectCount").get<size_t>());
	_e.SetEmitInstanceCount(_j.at("emitInstanceCount").get<size_t>());
	_e.SetBlendMode(static_cast<Effect::BlendMode>(_j.at("blendMode").get<int>()));
}

void ONEngine::to_json(nlohmann::json& _j, const Effect& _e) {
	_j = nlohmann::json{
		{ "type", "Effect" },
		{ "enable", _e.enable },
		{ "isCreateParticle", _e.IsCreateParticle() },
		{ "meshPath", _e.GetMeshPath() },
		{ "texturePath", _e.GetTexturePath() },
		{ "mainModule", _e.GetMainModule() },
		{ "emitShape", _e.GetEmitShape() },
		{ "emitType", _e.GetEmitType() },
		{ "distanceEmitData", _e.GetDistanceEmitData() },
		{ "timeEmitData", _e.GetTimeEmitData() },
		{ "maxEffectCount", _e.GetMaxEffectCount() },
		{ "emitInstanceCount", _e.GetEmitInstanceCount() },
		{ "blendMode", static_cast<int>(_e.GetBlendMode()) },
	};
}

void ONEngine::from_json(const nlohmann::json& _j, Effect::DistanceEmitData& _e) {
	_e.emitDistance = _j.at("emitDistance").get<float>();
	_e.emitInterval = _j.at("emitInterval").get<float>();
}

void ONEngine::to_json(nlohmann::json& _j, const Effect::DistanceEmitData& _e) {
	_j = nlohmann::json{
		{ "emitDistance", _e.emitDistance },
		{ "emitInterval", _e.emitInterval }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, Effect::TimeEmitData& _e) {
	_e.emitTime = _j.at("emitTime").get<float>();
	_e.emitInterval = _j.at("emitInterval").get<float>();
}

void ONEngine::to_json(nlohmann::json& _j, const Effect::TimeEmitData& _e) {
	_j = nlohmann::json{
		{ "emitTime", _e.emitTime },
		{ "emitInterval", _e.emitInterval }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, EffectMainModule& _e) {
	_e.SetLifeLeftTime(_j.at("lifeLeftTime").get<float>());
	_e.SetSpeedStartData(_j.at("startSpeed").get<std::pair<float, float>>());
	_e.SetSizeStartData(_j.at("startSize").get<std::pair<Vector3, Vector3>>());
	_e.SetRotateStartData(_j.at("startRotate").get<std::pair<Vector3, Vector3>>());
	_e.SetColorStartData(_j.at("startColor").get<std::pair<Color, Color>>());
	_e.SetGravityModifier(_j.at("gravityModifier").get<float>());
}

void ONEngine::to_json(nlohmann::json& _j, const EffectMainModule& _e) {
	_j = nlohmann::json{
		{ "lifeLeftTime", _e.GetLifeLeftTime() },
		{ "startSpeed", _e.GetSpeedStartData() },
		{ "startSize", _e.GetSizeStartData() },
		{ "startRotate", _e.GetRotateStartData() },
		{ "startColor", _e.GetColorStartData() },
		{ "gravityModifier", _e.GetGravityModifier() }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, EffectEmitShape& _e) {
	int type = _j.at("type").get<int>();
	switch (type) {
	case static_cast<int>(EffectEmitShape::ShapeType::Sphere):
		_e.SetSphere(_j.at("sphere").get<Sphere>());
		break;
	case static_cast<int>(EffectEmitShape::ShapeType::Cube):
		_e.SetCube(_j.at("cube").get<Cube>());
		break;
	case static_cast<int>(EffectEmitShape::ShapeType::Cone):
		_e.SetCone(_j.at("cone").get<Cone>());
		break;
	default:
		break;
	}
}

void ONEngine::to_json(nlohmann::json& _j, const EffectEmitShape& _e) {
	_j = nlohmann::json{
		{ "type", static_cast<int>(_e.GetType()) },
		{ "sphere", _e.GetSphere() },
		{ "cube", _e.GetCube() },
		{ "cone", _e.GetCone() }
	};
}


void ONEngine::from_json(const nlohmann::json& _j, CustomMeshRenderer& _m) {
	_m.enable = _j.at("enable").get<int>();
	_m.SetTexturePath(_j.at("texturePath").get<std::string>());
	_m.SetColor(_j.at("color").get<Color>());
}
void ONEngine::to_json(nlohmann::json& _j, const CustomMeshRenderer& _m) {
	_j = nlohmann::json{
		{ "type", "CustomMeshRenderer" },
		{ "enable", _m.enable },
		{ "texturePath", _m.GetTexturePath() },
		{ "color", _m.GetColor() }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, Line2DRenderer& _l) {
	_l.enable = _j.at("enable").get<int>();
}

void ONEngine::to_json(nlohmann::json& _j, const Line2DRenderer& _l) {
	_j = nlohmann::json{
		{ "type", "Line2DRenderer" },
		{ "enable", _l.enable }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, Line3DRenderer& _l) {
	if (!_j.contains("enable")) {
		_l.enable = _j.at("enable").get<int>();
	}
}

void ONEngine::to_json(nlohmann::json& _j, const Line3DRenderer& _l) {
	_j = nlohmann::json{
		{ "type", "Line3DRenderer" },
		{ "enable", _l.enable }
	};
}

// ParticleSystem

void ONEngine::from_json(const nlohmann::json& _j, AnimationCurveKey& _k) {
	_k.time = _j.value("time", 0.0f);
	_k.value = _j.value("value", 1.0f);
}
void ONEngine::to_json(nlohmann::json& _j, const AnimationCurveKey& _k) {
	_j = nlohmann::json{ {"time", _k.time}, {"value", _k.value} };
}

void ONEngine::from_json(const nlohmann::json& _j, AnimationCurve& _c) {
	_c.keys = _j.value("keys", std::vector<AnimationCurveKey>());
}
void ONEngine::to_json(nlohmann::json& _j, const AnimationCurve& _c) {
	_j = nlohmann::json{ {"keys", _c.keys} };
}

void ONEngine::from_json(const nlohmann::json& _j, MinMaxCurve& _m) {
	_m.state = static_cast<MinMaxState>(_j.value("state", static_cast<uint8_t>(MinMaxState::Constant)));
	_m.constant = _j.value("constant", 1.0f);
	_m.curve = _j.value("curve", AnimationCurve());
	_m.curveMin = _j.value("curveMin", AnimationCurve());
	_m.curveMax = _j.value("curveMax", AnimationCurve());
}
void ONEngine::to_json(nlohmann::json& _j, const MinMaxCurve& _m) {
	_j = nlohmann::json{
		{"state", static_cast<uint8_t>(_m.state)},
		{"constant", _m.constant},
		{"curve", _m.curve},
		{"curveMin", _m.curveMin},
		{"curveMax", _m.curveMax}
	};
}

void ONEngine::from_json(const nlohmann::json& _j, GradientColorKey& _k) {
	_k.color = _j.value("color", Color::kWhite);
	_k.time = _j.value("time", 0.0f);
}
void ONEngine::to_json(nlohmann::json& _j, const GradientColorKey& _k) {
	_j = nlohmann::json{ {"color", _k.color}, {"time", _k.time} };
}

void ONEngine::from_json(const nlohmann::json& _j, GradientAlphaKey& _k) {
	_k.alpha = _j.value("alpha", 1.0f);
	_k.time = _j.value("time", 0.0f);
}
void ONEngine::to_json(nlohmann::json& _j, const GradientAlphaKey& _k) {
	_j = nlohmann::json{ {"alpha", _k.alpha}, {"time", _k.time} };
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystemGradient& _g) {
	_g.colorKeys = _j.value("colorKeys", std::vector<GradientColorKey>());
	_g.alphaKeys = _j.value("alphaKeys", std::vector<GradientAlphaKey>());
}
void ONEngine::to_json(nlohmann::json& _j, const ParticleSystemGradient& _g) {
	_j = nlohmann::json{ {"colorKeys", _g.colorKeys}, {"alphaKeys", _g.alphaKeys} };
}

void ONEngine::from_json(const nlohmann::json& _j, MinMaxGradient& _m) {
	_m.state = static_cast<MinMaxState>(_j.value("state", static_cast<uint8_t>(MinMaxState::Constant)));
	_m.gradient = _j.value("gradient", ParticleSystemGradient());
	_m.gradientMin = _j.value("gradientMin", ParticleSystemGradient());
	_m.gradientMax = _j.value("gradientMax", ParticleSystemGradient());
}
void ONEngine::to_json(nlohmann::json& _j, const MinMaxGradient& _m) {
	_j = nlohmann::json{
		{"state", static_cast<uint8_t>(_m.state)},
		{"gradient", _m.gradient},
		{"gradientMin", _m.gradientMin},
		{"gradientMax", _m.gradientMax}
	};
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystem& _p) {
	_p.enable = _j.value("enable", 1);
	if (_j.contains("main")) _p.main = _j.at("main").get<ParticleSystemMain>();
	if (_j.contains("emission")) _p.emission = _j.at("emission").get<ParticleSystemEmission>();
	if (_j.contains("shape")) _p.shape = _j.at("shape").get<ParticleSystemShape>();
	if (_j.contains("colorOverLifetime")) _p.colorOverLifetime = _j.at("colorOverLifetime").get<ParticleSystemColorOverLifetime>();
	if (_j.contains("sizeOverLifetime")) _p.sizeOverLifetime = _j.at("sizeOverLifetime").get<ParticleSystemSizeOverLifetime>();
	if (_j.contains("velocityOverLifetime")) _p.velocityOverLifetime = _j.at("velocityOverLifetime").get<ParticleSystemVelocityOverLifetime>();
	if (_j.contains("renderer")) _p.renderer = _j.at("renderer").get<ParticleSystemRenderer>();
}

void ONEngine::to_json(nlohmann::json& _j, const ParticleSystem& _p) {
	_j = nlohmann::json{
		{ "type", "ParticleSystem" },
		{ "enable", _p.enable },
		{ "main", _p.main },
		{ "emission", _p.emission },
		{ "shape", _p.shape },
		{ "colorOverLifetime", _p.colorOverLifetime },
		{ "sizeOverLifetime", _p.sizeOverLifetime },
		{ "velocityOverLifetime", _p.velocityOverLifetime },
		{ "renderer", _p.renderer },
	};
}

void ONEngine::from_json(const nlohmann::json& _j, MinMaxFloat& _m) {
	_m.state = static_cast<MinMaxState>(_j.value("state", static_cast<uint8_t>(MinMaxState::Constant)));
	_m.constant = _j.value("constant", 0.0f);
	_m.minVal = _j.value("min", 0.0f);
	_m.maxVal = _j.value("max", 1.0f);
}

void ONEngine::to_json(nlohmann::json& _j, const MinMaxFloat& _m) {
	_j = nlohmann::json{
		{ "state", static_cast<uint8_t>(_m.state) },
		{ "constant", _m.constant },
		{ "min", _m.minVal },
		{ "max", _m.maxVal }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, MinMaxColor& _m) {
	_m.state = static_cast<MinMaxState>(_j.value("state", static_cast<uint8_t>(MinMaxState::Constant)));
	_m.constant = _j.value("constant", Color::kWhite);
	_m.minVal = _j.value("min", Color::kWhite);
	_m.maxVal = _j.value("max", Color::kWhite);
}

void ONEngine::to_json(nlohmann::json& _j, const MinMaxColor& _m) {
	_j = nlohmann::json{
		{ "state", static_cast<uint8_t>(_m.state) },
		{ "constant", _m.constant },
		{ "min", _m.minVal },
		{ "max", _m.maxVal }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystemMain& _m) {
	_m.duration = _j.value("duration", 5.0f);
	_m.looping = _j.value("looping", true);
	_m.prewarm = _j.value("prewarm", false);
	_m.startDelay = _j.value("startDelay", MinMaxFloat(0.0f));
	_m.startLifetime = _j.value("startLifetime", MinMaxFloat(5.0f));
	_m.startSpeed = _j.value("startSpeed", MinMaxFloat(5.0f));
	_m.startSize = _j.value("startSize", MinMaxFloat(1.0f));
	_m.startRotation = _j.value("startRotation", MinMaxFloat(0.0f));
	_m.startColor = _j.value("startColor", MinMaxColor(Color::kWhite));
	_m.gravityModifier = _j.value("gravityModifier", 0.0f);
	_m.simulationSpace = static_cast<SimulationSpace>(_j.value("simulationSpace", static_cast<uint8_t>(SimulationSpace::Local)));
	_m.maxParticles = _j.value("maxParticles", 1000);
	_m.playOnAwake = _j.value("playOnAwake", true);
}

void ONEngine::to_json(nlohmann::json& _j, const ParticleSystemMain& _m) {
	_j = nlohmann::json{
		{ "duration", _m.duration },
		{ "looping", _m.looping },
		{ "prewarm", _m.prewarm },
		{ "startDelay", _m.startDelay },
		{ "startLifetime", _m.startLifetime },
		{ "startSpeed", _m.startSpeed },
		{ "startSize", _m.startSize },
		{ "startRotation", _m.startRotation },
		{ "startColor", _m.startColor },
		{ "gravityModifier", _m.gravityModifier },
		{ "simulationSpace", static_cast<uint8_t>(_m.simulationSpace) },
		{ "maxParticles", _m.maxParticles },
		{ "playOnAwake", _m.playOnAwake }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystemEmission& _e) {
	_e.enabled = _j.value("enabled", true);
	_e.rateOverTime = _j.value("rateOverTime", 10.0f);
	_e.rateOverDistance = _j.value("rateOverDistance", 0.0f);
	_e.bursts = _j.value("bursts", std::vector<ParticleSystemEmission::Burst>());
}

void ONEngine::to_json(nlohmann::json& _j, const ParticleSystemEmission& _e) {
	_j = nlohmann::json{
		{ "enabled", _e.enabled },
		{ "rateOverTime", _e.rateOverTime },
		{ "rateOverDistance", _e.rateOverDistance },
		{ "bursts", _e.bursts }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystemEmission::Burst& _b) {
	_b.time = _j.value("time", 0.0f);
	_b.count = _j.value("count", 30);
	_b.cycles = _j.value("cycles", 1);
	_b.interval = _j.value("interval", 0.01f);
	_b.probability = _j.value("probability", 1.0f);
}

void ONEngine::to_json(nlohmann::json& _j, const ParticleSystemEmission::Burst& _b) {
	_j = nlohmann::json{
		{ "time", _b.time },
		{ "count", _b.count },
		{ "cycles", _b.cycles },
		{ "interval", _b.interval },
		{ "probability", _b.probability }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystemShape& _s) {
	_s.enabled = _j.value("enabled", true);
	_s.type = static_cast<ParticleSystemShapeType>(_j.value("type", static_cast<uint8_t>(ParticleSystemShapeType::Sphere)));
	_s.radius = _j.value("radius", 1.0f);
	_s.radiusThickness = _j.value("radiusThickness", 1.0f);
	_s.arc = _j.value("arc", 360.0f);
	_s.angle = _j.value("angle", 25.0f);
	_s.boxScale = _j.value("boxScale", Vector3(1.0f, 1.0f, 1.0f));
}

void ONEngine::to_json(nlohmann::json& _j, const ParticleSystemShape& _s) {
	_j = nlohmann::json{
		{ "enabled", _s.enabled },
		{ "type", static_cast<uint8_t>(_s.type) },
		{ "radius", _s.radius },
		{ "radiusThickness", _s.radiusThickness },
		{ "arc", _s.arc },
		{ "angle", _s.angle },
		{ "boxScale", _s.boxScale }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystemRenderer& _r) {
	_r.renderMode = static_cast<ParticleSystemRenderer::RenderMode>(_j.value("renderMode", static_cast<uint8_t>(ParticleSystemRenderer::RenderMode::Billboard)));
	_r.blendMode = static_cast<ParticleSystemRenderer::BlendMode>(_j.value("blendMode", static_cast<uint8_t>(ParticleSystemRenderer::BlendMode::Normal)));
	_r.materialGuid = _j.value("materialGuid", "");
	_r.meshGuid = _j.value("meshGuid", "");
}

void ONEngine::to_json(nlohmann::json& _j, const ParticleSystemRenderer& _r) {
	_j = nlohmann::json{
		{ "renderMode", static_cast<uint8_t>(_r.renderMode) },
		{ "blendMode", static_cast<uint8_t>(_r.blendMode) },
		{ "materialGuid", _r.materialGuid },
		{ "meshGuid", _r.meshGuid }
	};
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystemColorOverLifetime& _c) {
	_c.enabled = _j.value("enabled", false);
	_c.color = _j.value("color", MinMaxGradient());
}

void ONEngine::to_json(nlohmann::json& _j, const ParticleSystemColorOverLifetime& _c) {
	_j = nlohmann::json{ { "enabled", _c.enabled }, { "color", _c.color } };
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystemSizeOverLifetime& _s) {
	_s.enabled = _j.value("enabled", false);
	_s.size = _j.value("size", MinMaxCurve());
}

void ONEngine::to_json(nlohmann::json& _j, const ParticleSystemSizeOverLifetime& _s) {
	_j = nlohmann::json{ { "enabled", _s.enabled }, { "size", _s.size } };
}

void ONEngine::from_json(const nlohmann::json& _j, ParticleSystemVelocityOverLifetime& _v) {
	_v.enabled = _j.value("enabled", false);
	_v.x = _j.value("x", MinMaxCurve());
	_v.y = _j.value("y", MinMaxCurve());
	_v.z = _j.value("z", MinMaxCurve());
	_v.speedModifier = _j.value("speedModifier", MinMaxCurve());
	_v.space = static_cast<SimulationSpace>(_j.value("space", static_cast<uint8_t>(SimulationSpace::Local)));
}

void ONEngine::to_json(nlohmann::json& _j, const ParticleSystemVelocityOverLifetime& _v) {
	_j = nlohmann::json{
		{ "enabled", _v.enabled },
		{ "x", _v.x },
		{ "y", _v.y },
		{ "z", _v.z },
		{ "speedModifier", _v.speedModifier },
		{ "space", static_cast<uint8_t>(_v.space) }
	};
}

