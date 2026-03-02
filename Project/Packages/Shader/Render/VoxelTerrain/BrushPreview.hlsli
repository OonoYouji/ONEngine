#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "VoxelTerrainCommon.hlsli"
#include "LODInfo.hlsli"

struct VertexOut {
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION0;
    float3 normal : NORMAL0;
};

struct Payload {
	uint chunkIndex;
	float3 chunkOrigin;
	uint3 subChunkSize;
	uint chunkDivision;

    uint32_t3 chunkSize;
	uint lodLevel;
    float32_t3 brushWorldPos;
};


struct BrushInfo {
    float32_t2 mouseUV;
    uint32_t brushRadius;
    float32_t brushStrength;
};


ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection>   viewProjection   : register(b1);
ConstantBuffer<Camera>           camera           : register(b2);
ConstantBuffer<BrushInfo>        BrushInfo        : register(b5);

StructuredBuffer<Chunk> chunks : register(t0);
