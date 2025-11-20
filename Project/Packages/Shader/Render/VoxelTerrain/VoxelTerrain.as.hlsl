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
[numthreads(1, 1, 1)]
void main(
    uint3 DTid : SV_DispatchThreadID,
	uint3 groupId : SV_GroupID,
	uint groupIndex : SV_GroupIndex) {

	uint3 dispatchSize = uint3(0, 0, 0);
	Payload asPayload;
		
	/// チャンクの原点を計算
	uint3 chunkGridPosition = uint3(groupId.x, groupId.y, groupId.z);
	asPayload.chunkOrigin = float3(chunkGridPosition) * voxelTerrainInfo.chunkSize;

	//uint3 tempDispatchSize = uint3(0, 0, 0);

	AABB aabb;
	aabb.min = asPayload.chunkOrigin;
	aabb.max = asPayload.chunkOrigin + float3(voxelTerrainInfo.chunkSize);
	if (IsVisible(aabb, CreateFrustumFromMatrix(viewProjection.matVP))) {
		asPayload.chunkIndex = groupIndex;

		asPayload.subChunkSize = uint3(4, 4, 4);
		dispatchSize = voxelTerrainInfo.chunkSize / asPayload.subChunkSize;
	
		asPayload.dispatchSize = dispatchSize;
		
		//tempDispatchSize = uint3(1, 1, 1);

	}
	
	/// 分割された個数でディスパッチ
	DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, asPayload);
	//DispatchMesh(tempDispatchSize.x, tempDispatchSize.y, tempDispatchSize.z, asPayload);
	//DispatchMesh(1, 1, 1, asPayload);
}