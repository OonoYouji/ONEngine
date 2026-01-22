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
// 修正: chunkSizeを float -> float32_t3 に変更
uint32_t GetTransitionMask(float32_t3 chunkCenter, float32_t3 chunkSize, uint32_t myLOD, float32_t3 cameraPos)
{
    uint32_t mask = 0;
    
    // 判定用のヘルパー関数（ラムダ式が使えないので直接計算）
    // 隣接チャンクの中心位置とサイズから、そのチャンクの「カメラ最近点」を求めてLOD計算する
    
    float32_t3 halfSize = chunkSize / 2.0f;
    float32_t3 offset = chunkSize; // 隣接へのオフセット（サイズ分だけ移動）

    // ------------------------------------------------------
    // X軸方向 (-X: Left)
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter - float32_t3(offset.x, 0, 0);
        
        // 隣接チャンクのAABB範囲
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        
        // カメラから隣接AABBへの最近点 (nearPos) を計算 ★ここが修正ポイント
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax);
        
        uint32_t lodNX = CalculateLOD(nNearPos, cameraPos);
        if (lodNX > myLOD) mask |= TRANSITION_NX;
    }

    // ------------------------------------------------------
    // X軸方向 (+X: Right)
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter + float32_t3(offset.x, 0, 0);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax); // ★最近点を使う
        
        uint32_t lodPX = CalculateLOD(nNearPos, cameraPos);
        if (lodPX > myLOD) mask |= TRANSITION_PX;
    }

    // ------------------------------------------------------
    // Z軸方向 (-Z: Back)
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter - float32_t3(0, 0, offset.z);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax); // ★最近点を使う
        
        uint32_t lodNZ = CalculateLOD(nNearPos, cameraPos);
        if (lodNZ > myLOD) mask |= TRANSITION_NZ;
    }

    // ------------------------------------------------------
    // Z軸方向 (+Z: Front)
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter + float32_t3(0, 0, offset.z);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax); // ★最近点を使う
        
        uint32_t lodPZ = CalculateLOD(nNearPos, cameraPos);
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