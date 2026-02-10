
#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "VoxelTerrainCommon.hlsli"

/// ---------------------------------------------------
/// Structs
/// ---------------------------------------------------

struct VertexOut {
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION0;
	float3 normal : NORMAL0;
	// float4 color : COLOR0;
};


struct Payload {
	uint chunkIndex;
	float3 chunkOrigin;
	uint3 subChunkSize;
	uint chunkDivision;

	// uint3 dispatchSize;
    uint32_t3 chunkSize;
	uint lodLevel;
    uint32_t transitionMask;
};


/// ---------------------------------------------------
/// VoxelTerrain Common Buffers
/// ---------------------------------------------------

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection>   viewProjection   : register(b1);
ConstantBuffer<Camera>           camera           : register(b2);
ConstantBuffer<LODInfo>          lodInfo          : register(b3);

StructuredBuffer<Chunk> chunks : register(t0);



uint32_t GetLOD(float32_t distanceToCamera) {
    if (distanceToCamera < lodInfo.lodDistance1) {
        return 0;
    } else if (distanceToCamera < lodInfo.lodDistance2) {
        return 1;
    } else if (distanceToCamera < lodInfo.lodDistance3) {
        return 2;
    } 
    return 3;
}

uint32_t GetSubChunkSize(uint32_t lodLevel) {
    return 2u << lodLevel;
}