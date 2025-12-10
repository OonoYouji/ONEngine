#include "UVTransform.h"

using namespace ONEngine;


/// ------------------------------------------------
/// UVTransform Json変換
/// ------------------------------------------------

void ONEngine::to_json(nlohmann::json& _j, const UVTransform& _uvTransform) {
	_j = nlohmann::json{
		{ "position", _uvTransform.position },
		{ "scale", _uvTransform.scale },
		{ "rotate", _uvTransform.rotate },
	};
}

void ONEngine::from_json(const nlohmann::json& _j, UVTransform& _uvTransform) {
	_uvTransform.position = _j.value("position", Vector2::kZero);
	_uvTransform.scale = _j.value("scale", Vector2::kOne);
	_uvTransform.rotate = _j.value("rotate", 0.0f);
}
