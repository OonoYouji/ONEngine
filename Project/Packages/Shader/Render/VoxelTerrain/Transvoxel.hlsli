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
    uint32_t chunkId;
    uint32_t lodLevel;
    // bit0:-X, bit1:+X, bit4:-Z, bit5:+Z (Y軸は省略)
    uint32_t transitionMask;
};


ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection> viewProjection : register(b1);
ConstantBuffer<Camera> camera : register(b2);

StructuredBuffer<Chunk> chunks : register(t0);