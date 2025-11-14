#include "VoxelTerrain.hlsli"


/// ---------------------------------------------------
/// Buffers
/// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState voxelSampler : register(s0);



/// ---------------------------------------------------
/// Main
/// ---------------------------------------------------

/// max numthreads: 1024
[shader("amplification")]
[numthreads(8, 8, 1)]
void main(
    uint3 DTid : SV_DispatchThreadID,
    uint groupId : SV_GroupID) {
    
	//uint3 uvw = DTid / voxelTerrainInfo.chunkSize;
	//uint textureId = chunks[groupId].textureId;

	Payload asPayload;
	asPayload.chunkIndex = groupId;
	//asPayload.chunkOrigin = float3(DTid);
	
	DispatchMesh(1, 1, 1, asPayload);
}