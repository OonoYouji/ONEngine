#include "VectorJsonConverter.h"

using json = nlohmann::json;

void from_json(const json& _j, Vec2& _v) {
	_v.x = _j.at("x").get<float>();
	_v.y = _j.at("y").get<float>();
}

void to_json(json& _j, const Vec2& _v) {
	_j = json{ { "x", _v.x }, { "y", _v.y } };
}

void from_json(const json& _j, Vec3& _v) {
	_v.x = _j.at("x").get<float>();
	_v.y = _j.at("y").get<float>();
	_v.z = _j.at("z").get<float>();
}

void to_json(json& _j, const Vec3& _v) {
	_j = json{ { "x", _v.x }, { "y", _v.y }, { "z", _v.z } };
}

void from_json(const json& _j, Vec4& _v) {
	_v.x = _j.at("x").get<float>();
	_v.y = _j.at("y").get<float>();
	_v.z = _j.at("z").get<float>();
	_v.w = _j.at("w").get<float>();
}

void to_json(json& _j, const Vec4& _v) {
	_j = json{ { "x", _v.x }, { "y", _v.y }, { "z", _v.z }, { "w", _v.w } };
}
