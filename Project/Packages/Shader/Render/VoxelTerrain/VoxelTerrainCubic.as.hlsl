#include "VoxelTerrain.hlsli"


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
	asPayload.chunkOrigin = float3(groupId) * voxelTerrainInfo.chunkSize + uint3(voxelTerrainInfo.terrainOrigin);

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
		if (lengthToCamera <= 1000.0f) {
			uint subChunkSizeValue;

			/// LOD レベルを lengthToCamera の値に基づいて設定
			if (lengthToCamera < 50.0f) {
				asPayload.lodLevel = 0; // 高詳細度
				subChunkSizeValue = 2;
			} else if (lengthToCamera < 100.0f) {
				asPayload.lodLevel = 1; // 中詳細度
				subChunkSizeValue = 4;
			} else if (lengthToCamera < 200.0f) {
				asPayload.lodLevel = 2; // 低詳細度
				subChunkSizeValue = 8;
			} else {
				asPayload.lodLevel = 3; // 低詳細度
				subChunkSizeValue = 16;
			}

			asPayload.chunkIndex = IndexOfMeshGroup(groupId, uint3(voxelTerrainInfo.chunkCountXZ.x, 1, voxelTerrainInfo.chunkCountXZ.y));
			asPayload.subChunkSize = uint3(subChunkSizeValue, subChunkSizeValue, subChunkSizeValue);
			dispatchSize = voxelTerrainInfo.textureSize / asPayload.subChunkSize / uint32_t3(2,2,2); // numthreads に合わせて分割
            
            asPayload.transitionMask = GetTransitionMask(center, float3(voxelTerrainInfo.chunkSize), asPayload.lodLevel, camera.position.xyz);
		}
	}

	/// 分割された個数でディスパッチ
	asPayload.dispatchSize = dispatchSize;
	DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, asPayload);
}