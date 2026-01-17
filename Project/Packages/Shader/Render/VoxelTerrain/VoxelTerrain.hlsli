
#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "VoxelTerrainCommon.hlsli"

/// ---------------------------------------------------
/// Structs
/// ---------------------------------------------------

struct VertexOut {
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION0;
	float3 normal : NORMAL0;
	float4 color : COLOR0;
};


struct Payload {
	uint chunkIndex;
	float3 chunkOrigin;
	uint3 subChunkSize;
	uint chunkDivision;

	uint3 dispatchSize;
	uint lodLevel;
};


struct CommandInfo {
	int3 dispatchSize;
};



/// ---------------------------------------------------
/// functions
/// ---------------------------------------------------

uint32_t IndexOfMeshGroup(uint32_t3 groupID, uint32_t3 dim) {
	return groupID.x
         + groupID.y * dim.x
         + groupID.z * (dim.x * dim.y);
}


/// ---------------------------------------------------
/// VoxelTerrain Common Buffers
/// ---------------------------------------------------

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection>   viewProjection   : register(b1);
ConstantBuffer<Camera>           camera           : register(b2);

StructuredBuffer<Chunk> chunks : register(t0);