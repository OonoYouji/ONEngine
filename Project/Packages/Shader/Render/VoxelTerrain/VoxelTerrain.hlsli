
#include "../../ConstantBufferData/ViewProjection.hlsli"

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


struct Payload {
	//uint3 chunkCoord;
	uint chunkIndex;
	float3 chunkOrigin;
};


/// ---------------------------------------------------
/// VoxelTerrain Common Buffers
/// ---------------------------------------------------

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection>   viewProjection   : register(b1);

StructuredBuffer<Chunk> chunks : register(t0);