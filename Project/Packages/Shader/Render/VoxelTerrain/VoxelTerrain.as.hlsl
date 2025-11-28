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

		/// ---------------------------------------------------
		/// LODレベルを決め、サブチャンクの大きさを設定、高~低解像度に対応する
		/// ---------------------------------------------------
		
		float3 center = (aabb.min + aabb.max) * 0.5;
		float3 nearPoint = float3(
			clamp(camera.position.x, aabb.min.x, aabb.max.x),
			clamp(camera.position.y, aabb.min.y, aabb.max.y),
			clamp(camera.position.z, aabb.min.z, aabb.max.z)
		);

		float3 diff = nearPoint - camera.position.xyz;
		float lengthToCamera = length(diff);

		uint subChunkSizeValue;

		/// LOD レベルを ndc.z の値に基づいて設定
		if (lengthToCamera < 100.0f) {
			asPayload.lodLevel = 0; // 高詳細度
			subChunkSizeValue = 2;

		} else if (lengthToCamera < 300.0f) {
			asPayload.lodLevel = 1; // 中詳細度
			subChunkSizeValue = 8;

		} else {
			asPayload.lodLevel = 2; // 低詳細度
			subChunkSizeValue = 16;
		}




		asPayload.chunkIndex = IndexOfMeshGroup(groupId, uint3(voxelTerrainInfo.chunkCountXZ.x, 1, voxelTerrainInfo.chunkCountXZ.y));

		asPayload.subChunkSize = uint3(subChunkSizeValue, subChunkSizeValue, subChunkSizeValue);
		dispatchSize = voxelTerrainInfo.chunkSize / asPayload.subChunkSize;
	
		asPayload.dispatchSize = dispatchSize;

	
	}
	
	/// 分割された個数でディスパッチ
	DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, asPayload);
}