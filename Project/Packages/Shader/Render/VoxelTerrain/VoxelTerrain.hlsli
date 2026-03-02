#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "VoxelTerrainCommon.hlsli"
#include "LODInfo.hlsli"

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
	uint3 subChunkSize;

    uint32_t3 chunkSize;
    uint32_t transitionMask;

    /// 追加
    uint32_t step;
    float32_t3 startPos; /// 開始位置
};


/// ---------------------------------------------------
/// VoxelTerrain Common Buffers
/// ---------------------------------------------------

ConstantBuffer<VoxelTerrainInfo>    voxelTerrainInfo    : register(b0);
ConstantBuffer<ViewProjection>      viewProjection      : register(b1);
ConstantBuffer<Camera>              camera              : register(b2);

StructuredBuffer<Chunk>             chunks              : register(t0);