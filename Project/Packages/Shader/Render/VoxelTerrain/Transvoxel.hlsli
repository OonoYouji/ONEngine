#pragma once

#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "VoxelTerrainCommon.hlsli"
#include "LODInfo.hlsli"

static const float kIsoLevel = 0.5f;

struct VertexOut {
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct Payload {
    uint32_t chunkID;
    uint32_t LODLevel;
    uint32_t transitionMask;
    float32_t3 chunkOrigin;
	uint32_t3 subChunkSize;

    // uint32_t chunkIDs[32];
    // uint32_t LODLevels[32];
    // uint32_t transitionMasks[32];
    // uint32_t activeCount;
};



ConstantBuffer<VoxelTerrainInfo>    voxelTerrainInfo    : register(b0);
ConstantBuffer<ViewProjection>      viewProjection      : register(b1);
ConstantBuffer<Camera>              camera              : register(b2);

StructuredBuffer<Chunk>             chunks              : register(t0);