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
	asPayload.chunkOrigin = float3(groupId) * voxelTerrainInfo.chunkSize;

	/// カリング判定、可視ならディスパッチサイズを設定
	AABB aabb;
	aabb.min = asPayload.chunkOrigin;
	aabb.max = asPayload.chunkOrigin + float3(voxelTerrainInfo.chunkSize);
	if (IsVisible(aabb, CreateFrustumFromMatrix(viewProjection.matVP))) {
		asPayload.chunkIndex = IndexOfMeshGroup(groupId, uint3(voxelTerrainInfo.chunkCountXZ.x, 1, voxelTerrainInfo.chunkCountXZ.y));

		asPayload.subChunkSize = uint3(2, 2, 2);
		dispatchSize = voxelTerrainInfo.chunkSize / asPayload.subChunkSize;
	
		asPayload.dispatchSize = dispatchSize;
	}
	
	/// 分割された個数でディスパッチ
	DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, asPayload);
	//DispatchMesh(1, 1, 1, asPayload);
}