/// ---------------------------------------------------
/// Structs
/// ---------------------------------------------------

struct VertexOut {
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION0;
	float3 normal : NORMAL0;
	float4 color : COLOR0;
};


struct Chunk {
    /// Texture3D Id
	uint textureId;
};

struct VoxelTerrainInfo {
	uint3 chunkSize;
	uint2 chunkCountXZ;
	uint maxChunkCount;
};


struct SubChunk {
	uint3 subChunkOrigin;
	uint3 subChunkSize;
};


struct Payload {
	uint chunkIndex;
	float3 chunkOrigin;

	uint3 subChunkSize;

	uint3 dispatchSize;
};


struct CommandInfo {
	int3 dispatchSize;
};

