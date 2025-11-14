
/// ---------------------------------------------------
/// Structs
/// ---------------------------------------------------

struct VertexOut {
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION0;
	float3 normal : NORMAL0;
};


struct Chunk {
    /// Texture3D Id
	uint textureId;
};

struct VoxelTerrainInfo {
	float3 chunkSize;
	uint2 chunkCountXZ;
	uint maxChunkCount;
};


struct Payload {
	uint id;
};


/// ---------------------------------------------------
/// VoxelTerrain Common Buffers
/// ---------------------------------------------------

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
StructuredBuffer<Chunk> chunks : register(t0);