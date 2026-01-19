#include "Transvoxel.hlsli"



// -----------------------------------------------------------------------------
// ヘルパー：LOD計算関数
// -----------------------------------------------------------------------------
uint CalculateLOD(float32_t3 worldPos, float32_t3 cameraPos)
{
    float dist = distance(worldPos, cameraPos);
    
    // 例: 距離ベースの簡易LOD
    // 実際にはもっと細かく調整するか、事前計算されたLODマップを参照します
    if (dist < 50.0f)  return 0;
    if (dist < 100.0f) return 1;
    if (dist < 200.0f) return 2;
    return 3;
}

// -----------------------------------------------------------------------------
// メイン関数：Transition Mask の計算
// -----------------------------------------------------------------------------
// chunkCenter: 現在処理中のチャンクの中心ワールド座標
// chunkSize  : 現在処理中のチャンクの一辺のサイズ (LOD0基準ではなく、現在のスケールでのサイズ)
// myLOD      : 現在のチャンクのLODレベル
// cameraPos  : カメラ位置
// -----------------------------------------------------------------------------
uint32_t GetTransitionMask(float32_t3 chunkCenter, float32_t chunkSize, uint32_t myLOD, float32_t3 cameraPos)
{
    uint32_t mask = 0;

    // 隣接チャンクの中心位置を推定するためのオフセット距離
    // ※隣接チャンクも同じサイズであると仮定して中心を探ります。
    //   LODが切り替わる境界では、この点が「より粗いLODの領域」に含まれるかを判定することになります。
    float offsetDist = chunkSize; 

    // ------------------------------------------------------
    // X軸方向のチェック (-X, +X)
    // ------------------------------------------------------
    {
        // -X (Left)
        float32_t3 neighborPosNX = chunkCenter + float32_t3(-offsetDist, 0, 0);
        uint32_t lodNX = CalculateLOD(neighborPosNX, cameraPos);
        if (lodNX > myLOD) mask |= TRANSITION_NX;

        // +X (Right)
        float32_t3 neighborPosPX = chunkCenter + float32_t3(offsetDist, 0, 0);
        uint32_t lodPX = CalculateLOD(neighborPosPX, cameraPos);
        if (lodPX > myLOD) mask |= TRANSITION_PX;
    }

    // ------------------------------------------------------
    // Z軸方向のチェック (-Z, +Z)
    // ------------------------------------------------------
    {
        // -Z (Back)
        float32_t3 neighborPosNZ = chunkCenter + float32_t3(0, 0, -offsetDist);
        uint32_t lodNZ = CalculateLOD(neighborPosNZ, cameraPos);
        if (lodNZ > myLOD) mask |= TRANSITION_NZ;

        // +Z (Front)
        float32_t3 neighborPosPZ = chunkCenter + float32_t3(0, 0, offsetDist);
        uint32_t lodPZ = CalculateLOD(neighborPosPZ, cameraPos);
        if (lodPZ > myLOD) mask |= TRANSITION_PZ;
    }

    return mask;
}

// -----------------------------------------------------------------------------
// チャンクの中心点を取得する
// -----------------------------------------------------------------------------
float32_t3 GetChunkCenter(float32_t3 chunkOrigin) {
    return chunkOrigin + float32_t3(voxelTerrainInfo.chunkSize) * 0.5f;   
}

float32_t3 GetChunkOrigin(uint32_t3 groupID) {
    return float3(groupID) * voxelTerrainInfo.chunkSize + float3(voxelTerrainInfo.terrainOrigin);
}


// groupshared Payload sPayload;
// groupshared uint sVisibleCount;

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
            myMask = GetTransitionMask(chunkCenter, voxelTerrainInfo.chunkSize.x, myLOD, camera.position.xyz);
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
            
            p.subChunkSize = uint3(subChunkSize, subChunkSize, subChunkSize);
            dispatchSize = voxelTerrainInfo.textureSize / p.subChunkSize;
        }
    }
    
    DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, p);
}