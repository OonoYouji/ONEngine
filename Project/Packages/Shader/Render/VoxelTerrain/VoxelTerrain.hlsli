
#include "../../ConstantBufferData/ViewProjection.hlsli"
//#include "VoxelTerrainCommon.hlsli"

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
	uint lodLevel;
};


struct CommandInfo {
	int3 dispatchSize;
};



/// ---------------------------------------------------
/// functions
/// ---------------------------------------------------

uint IndexOfMeshGroup(uint3 _groupID, uint3 _dim) {
	return _groupID.x
         + _groupID.y * _dim.x
         + _groupID.z * (_dim.x * _dim.y);
}


/// ---------------------------------------------------
/// VoxelTerrain Common Buffers
/// ---------------------------------------------------

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection>   viewProjection   : register(b1);
ConstantBuffer<Camera>           camera           : register(b2);

StructuredBuffer<Chunk> chunks : register(t0);