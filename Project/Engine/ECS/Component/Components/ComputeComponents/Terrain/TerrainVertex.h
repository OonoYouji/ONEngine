#pragma once

#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// @brief 地形の頂点データ構造体
namespace ONEngine {

struct TerrainVertex {
	Vector4 position;
	Vector3 normal;
	Vector2 uv;
	Vector4 splatBlend;
	int index;
};

} /// ONEngine
