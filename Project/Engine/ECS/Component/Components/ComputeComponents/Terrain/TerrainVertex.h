#pragma once

#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// @brief 地形の頂点データ構造体
namespace ONEngine {

/**
 * @struct TerrainVertex
 * @brief 地形メッシュを構成する頂点情報（座標、法線、UV、テクスチャスプラットブレンドウェイト、インデックス）を保持する構造体
 */
struct TerrainVertex {
	Vector4 position;    ///< 座標
	Vector3 normal;      ///< 法線
	Vector2 uv;          ///< テクスチャUV座標
	Vector4 splatBlend;  ///< テクスチャスプラットマッピング用ブレンドウェイト
	int index;           ///< 頂点インデックス
};

} /// ONEngine
