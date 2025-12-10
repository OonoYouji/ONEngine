/// ---------------------------------------------------
/// Structs
/// ---------------------------------------------------


struct Chunk {
    /// Texture3D Id
	uint textureId;
};

struct VoxelTerrainInfo {
	float3 terrainOrigin;
	uint3 textureSize;
	uint3 chunkSize;
	uint2 chunkCountXZ;
	uint maxChunkCount;
};


struct SubChunk {
	uint3 subChunkOrigin;
	uint3 subChunkSize;
};
