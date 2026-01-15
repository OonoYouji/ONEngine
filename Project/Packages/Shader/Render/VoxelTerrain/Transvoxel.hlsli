#pragma once

#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "VoxelTerrainCommon.hlsli"

static const float kIsoLevel = 0.5f;

struct VertexOut {
    float4 positionCS : SV_POSITION;
    float4 worldPosition : POSITION0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct Payload {
    uint chunkIndex;       
    float3 chunkOrigin;    
    uint3 subChunkSize;    
    uint lodLevel;         
    
    // bit0:-X, bit1:+X, bit4:-Z, bit5:+Z (Y軸は省略)
    uint transitionMask;   
    
    // 1面あたりのセル解像度 (例: 16)
    // これがあれば MS側で (TotalID / (Res*Res)) して面IDを割り出せます
    uint faceResolution;   
};
// struct TransvoxelVertexOut {
// 	float32_t4 position : SV_POSITION;
// 	float32_t4 worldPos : POSITION0;
// 	float32_t3 normal : NORMAL0;
// };

// struct TransvoxelPayload {
// 	/// 0=XP, 1=XN, 2=ZP, 3=ZN, 4=YP, 5=YN
// 	uint32_t face;
// 	uint32_t myLOD;
// 	uint32_t neighborLOD;
// 	uint32_t chunkId;
//     uint32_t neighborChunkId;
// 	uint32_t3 chunkOrigin;
// 	uint32_t cellCount;
// };

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection> viewProjection : register(b1);
ConstantBuffer<Camera> camera : register(b2);

StructuredBuffer<Chunk> chunks : register(t0);