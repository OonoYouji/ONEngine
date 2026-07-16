#pragma once

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Asset/Assets/Mesh/Mesh.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"


namespace ONEngine {

/**
 * @struct VoxelTerrainChunk
 * @brief ボクセル地形で分割管理される個々のチャンク（領域）データ。GPU側で生成された頂点バッファと頂点数を保持します。
 */
struct VoxelTerrainChunk {

	struct Vertex {
		Vector4 position;
		Vector3 normal;
	};

	StructuredBuffer<Vertex> rwVertices_;
	uint32_t vertexCount_;

};

} /// namespace ONEngine