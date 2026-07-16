#include "Primitive.h"

using namespace ONEngine;

/**
 * @brief jsonオブジェクトからSphere構造体へのデシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _s 変換先のSphere構造体の参照
 */
void ONEngine::from_json(const nlohmann::json& _j, Sphere& _s) {
	_j.at("center").get_to(_s.center);
	_j.at("radius").get_to(_s.radius);
}

/**
 * @brief Sphere構造体からjsonオブジェクトへのシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _s 変換元のSphere構造体
 */
void ONEngine::to_json(nlohmann::json& _j, const Sphere& _s) {
	_j = nlohmann::json{
		{ "center", _s.center },
		{ "radius", _s.radius }
	};
}

/**
 * @brief jsonオブジェクトからCube構造体へのデシリアライズを行います。
 */
void ONEngine::from_json(const nlohmann::json& _j, Cube& _c) {
	_j.at("center").get_to(_c.center);
	_j.at("size").get_to(_c.size);
}

/**
 * @brief Cube構造体からjsonオブジェクトへのシリアライズを行います。
 */
void ONEngine::to_json(nlohmann::json& _j, const Cube& _c) {
	_j = nlohmann::json{
		{ "center", _c.center },
		{ "size", _c.size }
	};
}

/**
 * @brief jsonオブジェクトからLine構造体へのデシリアライズを行います。
 */
void ONEngine::from_json(const nlohmann::json& _j, Line& _l) {
	_j.at("start").get_to(_l.start);
	_j.at("end").get_to(_l.end);
}

/**
 * @brief Line構造体からjsonオブジェクトへのシリアライズを行います。
 */
void ONEngine::to_json(nlohmann::json& _j, const Line& _l) {
	_j = nlohmann::json{
		{ "start", _l.start },
		{ "end", _l.end }
	};
}

/**
 * @brief jsonオブジェクトからRay構造体へのデシリアライズを行います。
 */
void ONEngine::from_json(const nlohmann::json& _j, Ray& _r) {
	_j.at("origin").get_to(_r.origin);
	_j.at("direction").get_to(_r.direction);
}

/**
 * @brief Ray構造体からjsonオブジェクトへのシリアライズを行います。
 */
void ONEngine::to_json(nlohmann::json& _j, const Ray& _r) {
	_j = nlohmann::json{
		{ "origin", _r.origin },
		{ "direction", _r.direction }
	};
}

/**
 * @brief jsonオブジェクトからPlane構造体へのデシリアライズを行います。
 */
void ONEngine::from_json(const nlohmann::json& _j, Plane& _p) {
	_j.at("normal").get_to(_p.normal);
	_j.at("d").get_to(_p.d);
}

/**
 * @brief Plane構造体からjsonオブジェクトへのシリアライズを行います。
 */
void ONEngine::to_json(nlohmann::json& _j, const Plane& _p) {
	_j = nlohmann::json{
		{ "normal", _p.normal },
		{ "d", _p.d }
	};
}

/**
 * @brief jsonオブジェクトからFrustum構造体へのデシリアライズを行います。
 */
void ONEngine::from_json(const nlohmann::json& _j, Frustum& _f) {
	_j.at("planes").get_to(_f.planes);
}

/**
 * @brief Frustum構造体からjsonオブジェクトへのシリアライズを行います。
 */
void ONEngine::to_json(nlohmann::json& _j, const Frustum& _f) {
	_j = nlohmann::json{
		{ "planes", _f.planes }
	};
}

/**
 * @brief jsonオブジェクトからCone構造体へのデシリアライズを行います。
 */
void ONEngine::from_json(const nlohmann::json& _j, Cone& _c) {
	_j.at("center").get_to(_c.center);
	_j.at("angle").get_to(_c.angle);
	_j.at("radius").get_to(_c.radius);
	_j.at("height").get_to(_c.height);
}

/**
 * @brief Cone構造体からjsonオブジェクトへのシリアライズを行います。
 */
void ONEngine::to_json(nlohmann::json& _j, const Cone& _c) {
	_j = nlohmann::json{
		{ "center", _c.center },
		{ "angle", _c.angle },
		{ "radius", _c.radius },
		{ "height", _c.height }
	};
}
