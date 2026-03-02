#pragma once

#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "VoxelTerrainCommon.hlsli"
#include "LODInfo.hlsli"

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
};

ConstantBuffer<VoxelTerrainInfo>    voxelTerrainInfo    : register(b0);
ConstantBuffer<ViewProjection>      viewProjection      : register(b1);
ConstantBuffer<Camera>              camera              : register(b2);

StructuredBuffer<Chunk>             chunks              : register(t0);

static const float3 kTransitionCornerOffsets[13] = {
    float3(-0.5, -0.5, -0.5), float3( 0.0, -0.5, -0.5), float3( 0.5, -0.5, -0.5),
    float3(-0.5,  0.0, -0.5), float3( 0.0,  0.0, -0.5), float3( 0.5,  0.0, -0.5),
    float3(-0.5,  0.5, -0.5), float3( 0.0,  0.5, -0.5), float3( 0.5,  0.5, -0.5),
    float3(-0.5, -0.5,  0.5), float3( 0.5, -0.5,  0.5), float3(-0.5,  0.5,  0.5), float3( 0.5,  0.5,  0.5)
};

static const uint3 kPermutation[6] = {
    uint3(2, 0, 1), // 0: -X面 
    uint3(2, 0, 1), // 1: +X面
    uint3(1, 2, 0), // 2: -Y面 
    uint3(1, 2, 0), // 3: +Y面
    uint3(0, 1, 2), // 4: -Z面 
    uint3(0, 1, 2)  // 5: +Z面
};

static const uint3 kInvert[6] = {
    uint3(0, 0, 0), // 0: -X面
    uint3(0, 0, 1), // 1: +X面 (Z反転)
    uint3(0, 0, 0), // 2: -Y面
    uint3(0, 0, 1), // 3: +Y面 (Z反転)
    uint3(0, 0, 0), // 4: -Z面
    uint3(0, 0, 1)  // 5: +Z面 (Z反転)
};