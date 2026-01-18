struct Chunk {
	uint textureId;
};

struct VoxelTerrainInfo {
	float32_t3 terrainOrigin;
	uint32_t3 textureSize;
	uint32_t3 chunkSize;
	uint32_t2 chunkCountXZ;
	uint32_t maxChunkCount;
};

struct SubChunk {
	uint32_t3 subChunkOrigin;
	uint32_t3 subChunkSize;
};

// 頂点オフセット (0-7) - 中心を原点とする版
static const float3 kCornerOffsets[8] = {
	float3(-0.5, -0.5, -0.5), float3(0.5, -0.5, -0.5),
	float3(0.5, 0.5, -0.5), float3(-0.5, 0.5, -0.5),
	float3(-0.5, -0.5, 0.5), float3(0.5, -0.5, 0.5),
	float3(0.5, 0.5, 0.5), float3(-0.5, 0.5, 0.5)
};

// エッジ接続情報
static const int2 kEdgeConnection[12] = {
	int2(0, 1), int2(1, 2), int2(2, 3), int2(3, 0), // Bottom
	int2(4, 5), int2(5, 6), int2(6, 7), int2(7, 4), // Top
	int2(0, 4), int2(1, 5), int2(2, 6), int2(3, 7) // Vertical
};
