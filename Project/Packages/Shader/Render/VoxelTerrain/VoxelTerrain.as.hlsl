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
[numthreads(8, 1, 8)]
void main(
    uint3 DTid : SV_DispatchThreadID,
    uint groupId : SV_GroupID) {

	uint3 dispatchSize = uint3(0, 0, 0);
	Payload asPayload;

	asPayload.chunkIndex = groupId;
	asPayload.chunkOrigin = float3(DTid.x, DTid.y, DTid.z) * voxelTerrainInfo.chunkSize;

	//uint3 chunkDivision = uint3(32, 64, 32);
	//asPayload.subChunkSize = voxelTerrainInfo.chunkSize / chunkDivision;
	asPayload.subChunkSize = uint3(4, 4, 4);
	dispatchSize = voxelTerrainInfo.chunkSize / asPayload.subChunkSize;
	
	asPayload.dispatchSize = dispatchSize;
	
	
	/// 分割された個数でディスパッチ
	DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, asPayload);
	//DispatchMesh(2, 1, 2, asPayload);
}