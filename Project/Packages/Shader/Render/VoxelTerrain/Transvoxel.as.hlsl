#include "Transvoxel.hlsli"


uint32_t IndexOfMeshGroup(uint32_t3 groupID, uint32_t3 dim) {
	return groupID.x
         + groupID.y * dim.x
         + groupID.z * (dim.x * dim.y);
}


// -----------------------------------------------------------------------------
// Transvoxel標準のビットマスク順序
// -----------------------------------------------------------------------------
static const uint32_t TRANSITION_NX = 0x01; // -X (Left)
static const uint32_t TRANSITION_PX = 0x02; // +X (Right)
static const uint32_t TRANSITION_NZ = 0x10; // -Z (Back)
static const uint32_t TRANSITION_PZ = 0x20; // +Z (Front)

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

float32_t3 GetChunkOrigin(uint32_t chunkID) {
    
    uint32_t x = chunkID % voxelTerrainInfo.chunkCountXZ.x;
    uint32_t z = chunkID / voxelTerrainInfo.chunkCountXZ.x;
    
    return float3(x, 0, z) * voxelTerrainInfo.chunkSize + float3(voxelTerrainInfo.terrainOrigin);
}


groupshared Payload sPayload;
groupshared uint sVisibleCount;

// -----------------------------------------------------------------------------
// Amplification Shader Main
// -----------------------------------------------------------------------------
[shader("amplification")]
[numthreads(1, 1, 1)] // 例: 1スレッド = 1チャンク
void main(
    uint32_t3 dtid : SV_DispatchThreadID,
    uint32_t3 gtid : SV_GroupThreadID,
    uint32_t3 gid : SV_GroupID) {

    if (gtid.x == 0) {
        sVisibleCount = 0;
    }
    GroupMemoryBarrierWithGroupSync();

    bool isVisible = false;
    uint32_t myLOD = 0;
    uint32_t myMask = 0;

    /// 起動したThreadが地形の範囲内かチェック
    bool isBounds = dtid.x < voxelTerrainInfo.maxChunkCount;

    if(isBounds) {

        if(dtid.x < voxelTerrainInfo.maxChunkCount) {
            /// チャンクの原点、中心点の計算
	        float32_t3 chunkOrigin = GetChunkOrigin(dtid.x);
    
            /// カリングの判定
            AABB aabb;
            aabb.min = chunkOrigin;
            aabb.max = chunkOrigin + float3(voxelTerrainInfo.chunkSize);
            if(IsVisible(aabb, CreateFrustumFromMatrix(viewProjection.matVP))) {
                isVisible = true;

                /// LODレベルの計算
                float32_t3 chunkCenter = GetChunkCenter(chunkOrigin);
                myLOD = CalculateLOD(chunkCenter, camera.position.xyz);

                /// トランジションマスクの計算
                myMask = GetTransitionMask(chunkCenter, voxelTerrainInfo.chunkSize.x, myLOD, camera.position.xyz);
            }
        }
    }

    if(isVisible && myMask != 0) {
        uint listIndex;
        // 共有カウンタを1増やし、自分が書き込む場所(listIndex)を取得
        InterlockedAdd(sVisibleCount, 1, listIndex);

        sPayload.chunkIDs[listIndex] = dtid.x;
        sPayload.LODLevels[listIndex] = myLOD;
        sPayload.transitionMasks[listIndex] = myMask;
    }

    GroupMemoryBarrierWithGroupSync();
    
    uint3 dispatchSize = uint32_t3(0,0,0);
    Payload p;
    
    if (gtid.x == 0)
    {
        uint count = sVisibleCount;

        p.activeCount = count;

        [unroll]
        for (uint i = 0; i < 32; ++i)
        {
            p.chunkIDs[i] = sPayload.chunkIDs[i];
            p.LODLevels[i] = sPayload.LODLevels[i];
            p.transitionMasks[i] = sPayload.transitionMasks[i];
        }

        /*
        * 1Meshlet = 4x4x4
        * X: chunkSize / 4 = x;
        * Y: chunkSize / 4 = y;
        * Z: chunkSize / 4 = z;
        * 合計 8x128x8 = 8192 Meshlet / チャンク
        */
        
        uint32_t lodLevel = 0;
        uint32_t lodScale = 1u << lodLevel;
        
        uint32_t div = 4 * lodScale;
        uint32_t countX = max(1u, voxelTerrainInfo.chunkSize.x / div);
        uint32_t countY = max(1u, voxelTerrainInfo.chunkSize.y / div);
        uint32_t countZ = max(1u, voxelTerrainInfo.chunkSize.z / div);

        uint32_t meshletsPerChunk = countX * countY * countZ;

        dispatchSize = uint3(meshletsPerChunk, count, 1);
    }
    
    DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, p);
}