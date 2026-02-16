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

    /// 
    float32_t3 startPos = float32_t3(DTid) * voxelTerrainInfo.chunkSize;
    asPayload.startPos = startPos;

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

		float lengthToCamera = length(nearPoint - camera.position.xyz);
		if (lengthToCamera <= lodInfo.maxDrawDistance) {

            if(lodInfo.useLod != 0) {
                asPayload.lodLevel = GetLOD(lengthToCamera);
            } else {
                asPayload.lodLevel = lodInfo.lod;
            }

			uint32_t subChunkSize = GetSubChunkSize(asPayload.lodLevel);
			asPayload.chunkIndex = IndexOfMeshGroup(groupId, uint3(voxelTerrainInfo.chunkCountXZ.x, 1, voxelTerrainInfo.chunkCountXZ.y));
			asPayload.subChunkSize = uint3(subChunkSize, subChunkSize, subChunkSize);
			dispatchSize = voxelTerrainInfo.textureSize / asPayload.subChunkSize;
            /// numthreads に合わせて分割
            dispatchSize.x = (dispatchSize.x * dispatchSize.y * dispatchSize.z) / 16;
            dispatchSize.y = 1;
            dispatchSize.z = 1;

            asPayload.transitionMask = GetTransitionMask(center, float3(voxelTerrainInfo.chunkSize), asPayload.lodLevel, camera.position.xyz);
            
		}
	}

	/// 分割された個数でディスパッチ
	asPayload.chunkSize = voxelTerrainInfo.textureSize / asPayload.subChunkSize;
	DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, asPayload);
}