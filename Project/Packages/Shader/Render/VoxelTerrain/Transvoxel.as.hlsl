// Transvoxel.as.hlsl
#include "Transvoxel.hlsli"

//--------------------------------------
// ヘルパー関数
//--------------------------------------
uint3 GetChunkGridPos(uint chunkId)
{
    uint x = chunkId % voxelTerrainInfo.chunkCountXZ.x;
    uint z = (chunkId / voxelTerrainInfo.chunkCountXZ.x) % voxelTerrainInfo.chunkCountXZ.y;
    uint y = chunkId / (voxelTerrainInfo.chunkCountXZ.x * voxelTerrainInfo.chunkCountXZ.y);
    return uint3(x, y, z);
}

float3 GetChunkWorldCenter(uint3 gridPos)
{
    float3 size = float3(voxelTerrainInfo.chunkSize);
    float3 centerOffset = size * 0.5f; 
    // VoxelSize = 1.0f 前提
    return voxelTerrainInfo.terrainOrigin + (float3(gridPos) * size) + centerOffset;
}

uint CalculateLOD(float3 worldPos, float3 cameraPos)
{
    float dist = distance(worldPos, cameraPos);
    // 距離閾値はプロジェクトに合わせて調整してください
    if (dist < 50.0f)  return 0; // 最も詳細
    if (dist < 100.0f) return 1;
    if (dist < 200.0f) return 2;
    return 3;                    // 最も粗い
}

//--------------------------------------
// Amplification Shader Main
//--------------------------------------
[shader("amplification")]
[numthreads(1, 1, 1)]
void main(uint gid : SV_GroupID)
{
    uint chunkId = gid;
    
    // 初期化：デフォルトでは何も描画しない
    uint dispatchCount = 0;
    Payload payload = (Payload)0;

    // 範囲チェック
    if (chunkId < voxelTerrainInfo.maxChunkCount)
    {
        // 1. 自身のLOD計算
        uint3 gridPos = GetChunkGridPos(chunkId);
        float3 worldCenter = GetChunkWorldCenter(gridPos);
        uint myLOD = CalculateLOD(worldCenter, camera.position.xyz);

        // 2. Transition Mask 計算 (隣接とのLOD差判定)
        uint transitionMask = 0;
        
        // チェックする方向: -X, +X, -Z, +Z
        int3 offsets[4] = { int3(-1, 0, 0), int3(1, 0, 0), int3(0, 0, -1), int3(0, 0, 1) };
        uint bits[4]    = { 1, 2, 16, 32 }; // 0x01, 0x02, 0x10, 0x20

        [unroll]
        for(int i=0; i<4; ++i)
        {
            int3 neighborGrid = int3(gridPos) + offsets[i];
            
            // 隣接がグリッド範囲内かチェック
            if(all(neighborGrid >= 0) && 
               neighborGrid.x < (int)voxelTerrainInfo.chunkCountXZ.x && 
               neighborGrid.z < (int)voxelTerrainInfo.chunkCountXZ.y)
            {
                float3 neighborCenter = GetChunkWorldCenter(uint3(neighborGrid));
                uint neighborLOD = CalculateLOD(neighborCenter, camera.position.xyz);
                
                // Transvoxelのルール: 「隣接の方がLODが粗い(値が大きい)」場合のみTransition処理が必要
                if(neighborLOD > myLOD)
                {
                    transitionMask |= bits[i];
                }
            }
        }

        // 3. ペイロードの設定
        payload.chunkId = chunkId;
        payload.lodLevel = myLOD;
        payload.transitionMask = transitionMask;

        // 4. Dispatch数の計算
        // LODに応じてステップサイズが変わる (Step = 2^LOD)
        // これにより処理するセル数が減るため、Meshlet数も調整する
        uint stepSize = 1u << myLOD;
        uint3 rawSize = voxelTerrainInfo.chunkSize; 
        
        // 境界処理のため、厳密には (Size-1)/Step かどうかの調整が必要ですが、
        // ここでは簡易的に有効セル数からグループ数を算出します
        uint3 effectiveCells = (rawSize - 1) / stepSize;
        uint totalCells = effectiveCells.x * effectiveCells.y * effectiveCells.z;
        
        // 1グループあたり最大128スレッドと仮定
        dispatchCount = totalCells;
    }

    // 制約通り、DispatchMeshはここで一度だけ呼び出す
    DispatchMesh(dispatchCount, 1, 1, payload);
}