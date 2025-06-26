#include "ComponentJsonConverter.h"

/// std
#include <unordered_map>
#include <typeindex>
#include <functional>



/// //////////////////////////////////////////////////
/// ComponentJsonConverter
/// //////////////////////////////////////////////////

namespace {
	class JsonConverter {
	public:

		JsonConverter() {
			Register<Transform>();
			Register<Variables>();
			Register<DirectionalLight>();
			Register<AudioSource>();
			Register<Effect>();
			Register<SpriteRenderer>();
			Register<CustomMeshRenderer>();
			Register<MeshRenderer>();
			Register<Line2DRenderer>();
			Register<Line3DRenderer>();
			Register<ToTerrainCollider>();
		}

		template <typename T>
		void Register() {
			converters_[typeid(T).hash_code()] = [](const IComponent* _component) {
				return *static_cast<const T*>(_component);
				};

			fromJsonConverters_[typeid(T).hash_code()] = [this](IComponent* _component, const nlohmann::json& _j) {
				*static_cast<T*>(_component) = _j.get<T>();
				};
		}

		using Converter = std::function<nlohmann::json(const IComponent*)>;
		using FromJsonConverter = std::function<void(IComponent*, const nlohmann::json&)>;
		std::unordered_map<size_t, Converter> converters_;
		std::unordered_map<size_t, FromJsonConverter> fromJsonConverters_;

	};

	JsonConverter jsonConverter;
}

nlohmann::json ComponentJsonConverter::ToJson(const IComponent* _component) {
	auto it = jsonConverter.converters_.find(typeid(*_component).hash_code());
	if (it == jsonConverter.converters_.end()) {
		return nlohmann::json{};
	}

	return it->second(_component);
}

void ComponentJsonConverter::FromJson(const nlohmann::json& _j, IComponent* _component) {
	const std::string& name = typeid(*_component).name();
	auto it = jsonConverter.fromJsonConverters_.find(typeid(*_component).hash_code());

	if (it == jsonConverter.fromJsonConverters_.end()) {
		Console::Log("ComponentJsonConverter: " + name + "の変換関数が登録されていません。");
		return;
	}

	it->second(_component, _j);
}


void from_json(const nlohmann::json& _j, Vec2& _v) {
	_v.x = _j.at("x").get<float>();
	_v.y = _j.at("y").get<float>();
}

void to_json(nlohmann::json& _j, const Vec2& _v) {
	_j = nlohmann::json{ { "x", _v.x }, { "y", _v.y } };
}

void from_json(const nlohmann::json& _j, Vec3& _v) {
	_v.x = _j.at("x").get<float>();
	_v.y = _j.at("y").get<float>();
	_v.z = _j.at("z").get<float>();
}

void to_json(nlohmann::json& _j, const Vec3& _v) {
	_j = nlohmann::json{ { "x", _v.x }, { "y", _v.y }, { "z", _v.z } };
}

void from_json(const nlohmann::json& _j, Vec4& _v) {
	_v.x = _j.at("x").get<float>();
	_v.y = _j.at("y").get<float>();
	_v.z = _j.at("z").get<float>();
	_v.w = _j.at("w").get<float>();
}

void to_json(nlohmann::json& _j, const Vec4& _v) {
	_j = nlohmann::json{ { "x", _v.x }, { "y", _v.y }, { "z", _v.z }, { "w", _v.w } };
}

void from_json(const nlohmann::json& _j, Quaternion& _q) {
	_q.x = _j.at("x").get<float>();
	_q.y = _j.at("y").get<float>();
	_q.z = _j.at("z").get<float>();
	_q.w = _j.at("w").get<float>();
}

void to_json(nlohmann::json& _j, const Quaternion& _q) {
	_j = nlohmann::json{ { "x", _q.x }, { "y", _q.y }, { "z", _q.z }, { "w", _q.w } };
}

void from_json(const nlohmann::json& _j, Color& _c) {
	_c.r = _j.at("r").get<float>();
	_c.g = _j.at("g").get<float>();
	_c.b = _j.at("b").get<float>();
	_c.a = _j.at("a").get<float>();
}

void to_json(nlohmann::json& _j, const Color& _c) {
	_j = nlohmann::json{ { "r", _c.r }, { "g", _c.g }, { "b", _c.b }, { "a", _c.a } };
}

void from_json(const nlohmann::json& _j, Transform& _t) {
	_t.position = _j.at("position").get<Vec3>();
	_t.rotate = _j.at("rotate").get<Vec3>();
	_t.scale = _j.at("scale").get<Vec3>();
}

void to_json(nlohmann::json& _j, const Transform& _t) {
	_j = nlohmann::json{
		{ "type", "Transform" },
		{ "enable", _t.enable },
		{ "position", _t.position },
		{ "rotate", _t.rotate },
		{ "scale", _t.scale }
	};
}

void from_json(const nlohmann::json& _j, DirectionalLight& _l) {
	_l.SetIntensity(_j.at("intensity").get<float>());
	_l.SetDirection(_j.at("direction").get<Vec3>());
	_l.SetColor(_j.at("color").get<Vec4>());
}

void to_json(nlohmann::json& _j, const DirectionalLight& _l) {
	_j = nlohmann::json{
		{ "type", "DirectionalLight" },
		{ "enable", _l.enable },
		{ "intensity", _l.GetIntensity() },
		{ "direction", _l.GetDirection() },
		{ "color", _l.GetColor() }
	};
}

void from_json(const nlohmann::json& _j, AudioSource& _a) {
	_a.SetVolume(_j.at("volume").get<float>());
	_a.SetPitch(_j.at("pitch").get<float>());
	_a.SetAudioPath(_j.at("path").get<std::string>());
}

void to_json(nlohmann::json& _j, const AudioSource& _a) {
	_j = nlohmann::json{
		{ "type", "AudioSource" },
		{ "enable", _a.enable },
		{ "volume", _a.GetVolume() },
		{ "pitch", _a.GetPitch() },
		{ "path", _a.GetAudioPath() }
	};
}

void from_json([[maybe_unused]] const nlohmann::json& _j, [[maybe_unused]] Variables& _v) {
	
}
void to_json(nlohmann::json& _j, const Variables& _v) {
	_j = nlohmann::json{
		{ "type", "Variables" },
		// 変数の内容はここで定義する必要があります
	};
}

void from_json(const nlohmann::json& _j, Effect& _e) {
	_e.enable = _j.at("enable").get<bool>();
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

void to_json(nlohmann::json& _j, const Effect& _e) {
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

void from_json(const nlohmann::json& _j, Effect::DistanceEmitData& _e) {
	_e.emitDistance = _j.at("emitDistance").get<float>();
	_e.emitInterval = _j.at("emitInterval").get<float>();
}

void to_json(nlohmann::json& _j, const Effect::DistanceEmitData& _e) {
	_j = nlohmann::json{
		{ "emitDistance", _e.emitDistance },
		{ "emitInterval", _e.emitInterval }
	};
}

void from_json(const nlohmann::json& _j, Effect::TimeEmitData& _e) {
	_e.emitTime = _j.at("emitTime").get<float>();
	_e.emitInterval = _j.at("emitInterval").get<float>();
}

void to_json(nlohmann::json& _j, const Effect::TimeEmitData& _e) {
	_j = nlohmann::json{
		{ "emitTime", _e.emitTime },
		{ "emitInterval", _e.emitInterval }
	};
}

void from_json(const nlohmann::json& _j, EffectMainModule& _e) {
	_e.SetLifeLeftTime(_j.at("lifeLeftTime").get<float>());
	_e.SetSpeedStartData(_j.at("startSpeed").get<std::pair<float, float>>());
	_e.SetSizeStartData(_j.at("startSize").get<std::pair<Vec3, Vec3>>());
	_e.SetRotateStartData(_j.at("startRotate").get<std::pair<Vec3, Vec3>>());
	_e.SetColorStartData(_j.at("startColor").get<std::pair<Color, Color>>());
	_e.SetGravityModifier(_j.at("gravityModifier").get<float>());
}

void to_json(nlohmann::json& _j, const EffectMainModule& _e) {
	_j = nlohmann::json{
		{ "lifeLeftTime", _e.GetLifeLeftTime() },
		{ "startSpeed", _e.GetSpeedStartData() },
		{ "startSize", _e.GetSizeStartData() },
		{ "startRotate", _e.GetRotateStartData() },
		{ "startColor", _e.GetColorStartData() },
		{ "gravityModifier", _e.GetGravityModifier() }
	};
}

void from_json(const nlohmann::json& _j, EffectEmitShape& _e) {
	int type = _j.at("type").get<int>();
	switch (type) {
	case static_cast<int>(EffectEmitShape::ShapeType::Sphere):
		_e.SetSphere(_j.at("sphere").get<EffectEmitShape::Sphere>());
		break;
	case static_cast<int>(EffectEmitShape::ShapeType::Cube):
		_e.SetCube(_j.at("cube").get<EffectEmitShape::Cube>());
		break;
	case static_cast<int>(EffectEmitShape::ShapeType::Cone):
		_e.SetCone(_j.at("cone").get<EffectEmitShape::Cone>());
		break;
	default:
		break;
	}
}

void to_json(nlohmann::json& _j, const EffectEmitShape& _e) {
	_j = nlohmann::json{
		{ "type", static_cast<int>(_e.GetType()) },
		{ "sphere", _e.GetSphere() },
		{ "cube", _e.GetCube() },
		{ "cone", _e.GetCone() }
	};
}

void from_json(const nlohmann::json& _j, EffectEmitShape::Sphere& _e) {
	_e.center = _j.at("center").get<Vec3>();
	_e.radius = _j.at("radius").get<float>();
}
void to_json(nlohmann::json& _j, const EffectEmitShape::Sphere& _e) {
	_j = nlohmann::json{
		{ "center", _e.center },
		{ "radius", _e.radius }
	};
}

void from_json(const nlohmann::json& _j, EffectEmitShape::Cube& _e) {
	_e.center = _j.at("center").get<Vec3>();
	_e.size = _j.at("size").get<Vec3>();
}
void to_json(nlohmann::json& _j, const EffectEmitShape::Cube& _e) {
	_j = nlohmann::json{
		{ "center", _e.center },
		{ "size", _e.size }
	};
}

void from_json(const nlohmann::json& _j, EffectEmitShape::Cone& _e) {
	_e.center = _j.at("center").get<Vec3>();
	_e.angle = _j.at("angle").get<float>();
	_e.radius = _j.at("radius").get<float>();
	_e.height = _j.at("height").get<float>();
}
void to_json(nlohmann::json& _j, const EffectEmitShape::Cone& _e) {
	_j = nlohmann::json{
		{ "center", _e.center },
		{ "angle", _e.angle },
		{ "radius", _e.radius },
		{ "height", _e.height }
	};
}

void from_json(const nlohmann::json& _j, MeshRenderer& _m) {
	_m.enable = _j.at("enable").get<bool>();
	_m.SetMeshPath(_j.at("meshPath").get<std::string>());
	_m.SetTexturePath(_j.at("texturePath").get<std::string>());
	_m.SetColor(_j.at("color").get<Vec4>());
}
void to_json(nlohmann::json& _j, const MeshRenderer& _m) {
	_j = nlohmann::json{
		{ "type", "MeshRenderer" },
		{ "enable", _m.enable },
		{ "meshPath", _m.GetMeshPath() },
		{ "texturePath", _m.GetTexturePath() },
		{ "color", _m.GetColor() }
	};
}

void from_json(const nlohmann::json& _j, CustomMeshRenderer& _m) {
	_m.enable = _j.at("enable").get<bool>();
	_m.SetTexturePath(_j.at("texturePath").get<std::string>());
	_m.SetColor(_j.at("color").get<Color>());
}
void to_json(nlohmann::json& _j, const CustomMeshRenderer& _m) {
	_j = nlohmann::json{
		{ "type", "CustomMeshRenderer" },
		{ "enable", _m.enable },
		{ "texturePath", _m.GetTexturePath() },
		{ "color", _m.GetColor() }
	};
}

void from_json(const nlohmann::json& _j, SpriteRenderer& _s) {
	_s.enable = _j.at("enable").get<bool>();
	_s.SetTexturePath(_j.at("texturePath").get<std::string>());
}

void to_json(nlohmann::json& _j, const SpriteRenderer& _s) {
	_j = nlohmann::json{
		{ "type", "SpriteRenderer" },
		{ "enable", _s.enable },
		{ "texturePath", _s.GetTexturePath() }
	};
}

void from_json(const nlohmann::json& _j, Line2DRenderer& _l) {
	_l.enable = _j.at("enable").get<bool>();
}

void to_json(nlohmann::json& _j, const Line2DRenderer& _l) {
	_j = nlohmann::json{
		{ "type", "Line2DRenderer" },
		{ "enable", _l.enable }
	};
}

void from_json(const nlohmann::json& _j, Line3DRenderer& _l) {
	_l.enable = _j.at("enable").get<bool>();
}

void to_json(nlohmann::json& _j, const Line3DRenderer& _l) {
	_j = nlohmann::json{
		{ "type", "Line3DRenderer" },
		{ "enable", _l.enable }
	};
}

void from_json(const nlohmann::json& _j, ToTerrainCollider& _c) {
	_c.enable = _j.at("enable").get<bool>();
}

void to_json(nlohmann::json& _j, const ToTerrainCollider& _c) {
	_j = nlohmann::json{
		{ "type", "ToTerrainCollider" },
		{ "enable", _c.enable }
	};
}

void from_json(const nlohmann::json& _j, Script& _s) {
	_s.enable = _j.at("enable").get<bool>();
	if (_j.contains("scriptName")) {

		/// スクリプト名が文字列または配列であることを確認
		if (_j["scriptName"].is_string()) {
			_s.AddScript(_j.at("scriptName").get<std::string>());
		} else if (_j["scriptName"].is_array()) {

			/// 配列の場合、各スクリプト名を追加
			for (const auto& name : _j.at("scriptName")) {
				_s.AddScript(name.get<std::string>());
			}
		}

	} else {
		Console::Log("Script component JSON does not contain 'scriptName'.");
	}
}

void to_json(nlohmann::json& _j, const Script& _s) {
	_j = nlohmann::json{
		{ "type", "Script" },
		{ "enable", _s.enable },
		{ "scriptName", _s.GetScriptNames() }
	};
}







