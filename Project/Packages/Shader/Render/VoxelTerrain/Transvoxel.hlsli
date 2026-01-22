#pragma once

#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "VoxelTerrainCommon.hlsli"

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


// -----------------------------------------------------------------------------
// Transvoxel標準のビットマスク順序
// -----------------------------------------------------------------------------
static const uint32_t TRANSITION_NX = 0x01; // -X (Left)
static const uint32_t TRANSITION_PX = 0x02; // +X (Right)
static const uint32_t TRANSITION_NY = 0x04; // -Y (Bottom)
static const uint32_t TRANSITION_PY = 0x08; // +Y (p)
static const uint32_t TRANSITION_NZ = 0x10; // -Z (Back)
static const uint32_t TRANSITION_PZ = 0x20; // +Z (Front)


ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection> viewProjection : register(b1);
ConstantBuffer<Camera> camera : register(b2);

StructuredBuffer<Chunk> chunks : register(t0);
