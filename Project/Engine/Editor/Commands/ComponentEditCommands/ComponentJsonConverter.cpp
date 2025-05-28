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
		}

		template <typename T>
		void Register() {
			converters_[typeid(T).hash_code()] = [](const IComponent* _component) {
				return *static_cast<const T*>(_component);
				};
		}

		using Converter = std::function<nlohmann::json(const IComponent*)>;
		std::unordered_map<size_t, Converter> converters_;
	};

	JsonConverter jsonConverter;
}

nlohmann::json ComponentJsonConverter::ToJson(const IComponent* _component) {
	auto it = jsonConverter.converters_.find(typeid(*_component).hash_code());
	if (it == jsonConverter.converters_.end()) {
	}

	return it->second(_component);
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
		{ "volume", _a.GetVolume() },
		{ "pitch", _a.GetPitch() },
		{ "path", _a.GetAudioPath() }
	};
}

void from_json(const nlohmann::json& _j, Variables& _v) {}
void to_json(nlohmann::json& _j, const Variables& _v) {}

void from_json(const nlohmann::json& _j, Effect& _e) {

}

void to_json(nlohmann::json& _j, const Effect& _e) {
	_j = nlohmann::json{
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

void from_json(const nlohmann::json& _j, Effect::DistanceEmitData& _e) {}

void to_json(nlohmann::json& _j, const Effect::DistanceEmitData& _e) {}

void from_json(const nlohmann::json& _j, Effect::TimeEmitData& _e) {}

void to_json(nlohmann::json& _j, const Effect::TimeEmitData& _e) {}

void from_json(const nlohmann::json& _j, EffectMainModule& _e) {}

void to_json(nlohmann::json& _j, const EffectMainModule& _e) {}

void from_json(const nlohmann::json& _j, EffectEmitShape& _e) {}

void to_json(nlohmann::json& _j, const EffectEmitShape& _e) {}


//void from_json(const nlohmann::json& _j, EffectMainModule& _m) {
//	_m.SetLifeLeftTime(_j.at("lifeLeftTime").get<float>());
//	_m.SetSizeStartData(_j.at("startSize").get<Vec3>());
//	_m.startRotate_ = _j.at("startRotate").get<Vec3>();
//	_m.startColor_ = _j.at("startColor").get<Color>();
//	_m.startSpeed_ = _j.at("startSpeed").get<float>();
//}
//
//void to_json(nlohmann::json& _j, const EffectMainModule& _m) {
//	_j = nlohmann::json{
//		{ "lifeLeftTime", _m.lifeLeftTime_ },
//		{ "startSize", _m.startSize_ },
//		{ "startRotate", _m.startRotate_ },
//		{ "startColor", _m.startColor_ },
//		{ "startSpeed", _m.startSpeed_ }
//	};
//}
//
