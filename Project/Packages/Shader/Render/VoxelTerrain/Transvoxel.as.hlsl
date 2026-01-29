#include "Transvoxel.hlsli"


// -----------------------------------------------------------------------------
// チャンクの中心点を取得する
// -----------------------------------------------------------------------------
float32_t3 GetChunkCenter(float32_t3 chunkOrigin) {
    return chunkOrigin + float32_t3(voxelTerrainInfo.chunkSize) * 0.5f;   
}

float32_t3 GetChunkOrigin(uint32_t3 groupID) {
    return float3(groupID) * voxelTerrainInfo.chunkSize + float3(voxelTerrainInfo.terrainOrigin);
}


// -----------------------------------------------------------------------------
// Amplification Shader Main
// -----------------------------------------------------------------------------
[shader("amplification")]
[numthreads(1, 1, 1)] // 例: 1スレッド = 1チャンク
void main(
    uint32_t3 dtid : SV_DispatchThreadID,
    uint32_t3 gtid : SV_GroupThreadID,
    uint32_t3 gid : SV_GroupID) {

    bool isVisible = false;
    uint32_t myLOD = 0;
    uint32_t myMask = 0;

    Payload p;
	float32_t3 chunkOrigin = GetChunkOrigin(gid);
    p.chunkOrigin = chunkOrigin;
    uint3 dispatchSize = uint32_t3(0,0,0);

    /// カリングの判定
    AABB aabb;
    aabb.min = chunkOrigin;
    aabb.max = chunkOrigin + float3(voxelTerrainInfo.chunkSize);
    if(IsVisible(aabb, CreateFrustumFromMatrix(viewProjection.matVP))) {

        /// LODレベルの計算
        float32_t3 chunkCenter = GetChunkCenter(chunkOrigin);
        float32_t3 nearPos = float32_t3(
            clamp(camera.position.x, aabb.min.x, aabb.max.x),
            clamp(camera.position.y, aabb.min.y, aabb.max.y),
            clamp(camera.position.z, aabb.min.z, aabb.max.z)
        );

        if(distance(nearPos, camera.position.xyz) <= 1000.0f) {
            isVisible = true;

            myLOD = CalculateLOD(nearPos, camera.position.xyz);

            /// トランジションマスクの計算
            myMask = GetTransitionMask(chunkCenter, float32_t3(voxelTerrainInfo.chunkSize), myLOD, camera.position.xyz);
            p.chunkID = IndexOfMeshGroup(gid, uint3(voxelTerrainInfo.chunkCountXZ.x, 1, voxelTerrainInfo.chunkCountXZ.y));
            p.LODLevel = myLOD;
            p.transitionMask = myMask;

            uint32_t lodLevel = p.LODLevel;
            uint32_t subChunkSize;

            /// LOD レベルを lengthToCamera の値に基づいて設定
		    if (lodLevel == 0) {
		    	subChunkSize = 2;
		    } else if (lodLevel == 1) {
		    	subChunkSize = 4;
		    } else if (lodLevel == 2) {
		    	subChunkSize = 8;
		    } else {
		    	subChunkSize = 16;
		    }
            
            if(myMask != 0) {
                p.subChunkSize = uint3(subChunkSize, subChunkSize, subChunkSize);
                dispatchSize = voxelTerrainInfo.textureSize / p.subChunkSize;
            }
        }
    }
    
    DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, p);
}