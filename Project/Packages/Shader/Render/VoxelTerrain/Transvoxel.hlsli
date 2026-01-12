#include "VoxelTerrainCommon.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"


struct VertexOut {
	float4 position : SV_POSITION;
	float4 worldPos : POSITION0;
	float3 normal : NORMAL0;
};

struct Payload {
	/// 0=XP, 1=XN, 2=ZP, 3=ZN, 4=YP, 5=YN
	uint face;
	uint myLOD;
	uint neighborLOD;
	uint pad0;
	uint3 chunkOrigin;
	uint cellCount;
};


ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection> viewProjection : register(b1);
ConstantBuffer<Camera> camera : register(b2);

StructuredBuffer<Chunk> chunks : register(t0);