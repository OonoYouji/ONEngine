#include "VectorJsonConverter.h"

using json = nlohmann::json;

void from_json(const json& j, Vec2& v) {
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();
}

void to_json(json& j, const Vec2& v) {
	j = json{ { "x", v.x }, { "y", v.y } };
}

void from_json(const json& j, Vec3& v) {
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();
	v.z = j.at("z").get<float>();
}

void to_json(json& j, const Vec3& v) {
	j = json{ { "x", v.x }, { "y", v.y }, { "z", v.z } };
}

void from_json(const json& j, Vec4& v) {
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();
	v.z = j.at("z").get<float>();
	v.w = j.at("w").get<float>();
}

void to_json(json& j, const Vec4& v) {
	j = json{ { "x", v.x }, { "y", v.y }, { "z", v.z }, { "w", v.w } };
}
