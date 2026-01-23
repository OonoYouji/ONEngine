#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"
#include "../VoxelTerrainTest/Table.hlsli"

// -----------------------------------------------------------------------------
// Resources
// -----------------------------------------------------------------------------
Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

// [解説] チャンクIDからワールド座標の原点を計算する関数
float32_t3 GetChunkPos(uint32_t chunkID) {
    uint32_t x = chunkID % voxelTerrainInfo.chunkCountXZ.x;
    uint32_t z = chunkID / voxelTerrainInfo.chunkCountXZ.x;
    
    return float32_t3(x, 0, z) * float32_t3(voxelTerrainInfo.chunkSize) + float32_t3(voxelTerrainInfo.terrainOrigin);
}

float32_t3 GetChunkCenter(uint32_t chunkID) {
    float32_t3 chunkPos = GetChunkPos(chunkID);
    return chunkPos + float32_t3(voxelTerrainInfo.chunkSize) / 2;
}

// [解説] 3Dテクスチャから密度値(SDF等)をサンプリングする関数
float32_t GetDensity(float32_t3 samplePos, uint32_t chunkID) {
    float voxelSize = 1.0f;
    float32_t3 textureSize = float32_t3(voxelTerrainInfo.textureSize);
    float32_t3 uvw = samplePos / (textureSize * voxelSize);

    /// Y軸反転
    uvw.y = 1.0f - uvw.y;

    // チャンクのグリッド座標を計算
    int chunkX = int(chunkID) % int(voxelTerrainInfo.chunkCountXZ.x);
    int chunkZ = int(chunkID) / int(voxelTerrainInfo.chunkCountXZ.x);

    uint32_t idx = chunkID;
    
    // X方向の境界処理
    if (uvw.x < 0.0f) {
        if (chunkX > 0) {
            idx = chunkID - 1;
            uvw.x += 1.0f;
        } else { return 0.0f; }
    } else if (uvw.x > 1.0f) {
        if (chunkX < int(voxelTerrainInfo.chunkCountXZ.x) - 1) {
            idx = chunkID + 1;
            uvw.x -= 1.0f;
        } else { return 0.0f; }
    }
    
    // Z方向の境界処理
    if (uvw.z < 0.0f) {
        if (chunkZ > 0) {
            idx -= uint(voxelTerrainInfo.chunkCountXZ.x);
            uvw.z += 1.0f;
        } else { return 0.0f; }
    } else if (uvw.z > 1.0f) {
        if (chunkZ < int(voxelTerrainInfo.chunkCountXZ.y) - 1) {
            idx += uint(voxelTerrainInfo.chunkCountXZ.x);
            uvw.z -= 1.0f;
        } else { return 0.0f; }
    }

    // Y方向の範囲外処理（空は空気、地下は固体）
    if (uvw.y < 0.0f) { return 1.0f; } // 地下
    if (uvw.y > 1.0f) { return 0.0f; } // 空

    uvw = saturate(uvw);

    float32_t4 rgba = voxelChunkTextures[chunks[idx].textureId].SampleLevel(texSampler, uvw, 0);
    return rgba.a;
}

// 法線計算用ヘルパー
float3 CalculateNormal(float3 worldPos, uint chunkID)
{
    float delta = 0.01;
    float3 grad;
    
    grad.x = GetDensity(worldPos + float3(delta, 0, 0), chunkID) - 
             GetDensity(worldPos - float3(delta, 0, 0), chunkID);
    grad.y = GetDensity(worldPos + float3(0, delta, 0), chunkID) - 
             GetDensity(worldPos - float3(0, delta, 0), chunkID);
    grad.z = GetDensity(worldPos + float3(0, 0, delta), chunkID) - 
             GetDensity(worldPos - float3(0, 0, delta), chunkID);
    
    return normalize(-grad);
}

// -----------------------------------------------------------------------------
// Transvoxel Definitions (Corrected)
// -----------------------------------------------------------------------------

// Transvoxel Canonical Offsets (Z-Major Order)
// Z軸方向(0->0.5->1)を先に進める順番。これでビットマスクが正しく「面」として認識されます。
static const float3 kTransitionCornerOffsets[13] = 
{
    // --- High-Resolution Face (x=0) [Indices 0-8] ---
    // (Local Y, Local Z)
    float3(0, 0, 0),     // 0: (0, 0)   Bottom-Left
    float3(0, 0, 0.5),   // 1: (0, 0.5) Bottom-Center
    float3(0, 0, 1),     // 2: (0, 1)   Bottom-Right
    
    float3(0, 0.5, 0),   // 3: (0.5, 0) Mid-Left
    float3(0, 0.5, 0.5), // 4: (0.5, 0.5) Center
    float3(0, 0.5, 1),   // 5: (0.5, 1) Mid-Right
    
    float3(0, 1, 0),     // 6: (1, 0)   Top-Left
    float3(0, 1, 0.5),   // 7: (1, 0.5) Top-Center
    float3(0, 1, 1),     // 8: (1, 1)   Top-Right

    // --- Low-Resolution Face (x=1) [Indices 9-12] ---
    float3(1, 0, 0),     // 9: (0, 0)
    float3(1, 0, 1),     // A: (0, 1) ... Z axis is fast
    float3(1, 1, 0),     // B: (1, 0)
    float3(1, 1, 1)      // C: (1, 1)
};

// 回転テーブル（Canonical X-Axis Thickness）
static const uint3 kPermutation[6] = {
    uint3(0, 1, 2), // 0: -X (Base)
    uint3(0, 1, 2), // 1: +X
    uint3(1, 2, 0), // 2: -Y (Rotate X-thickness to Y)
    uint3(1, 2, 0), // 3: +Y
    uint3(2, 0, 1), // 4: -Z (Rotate X-thickness to Z)
    uint3(2, 0, 1)  // 5: +Z
};

// 反転テーブル（重要：プラス方向の面は高解像度面を外側に向けるため反転が必要）
static const uint3 kInvert[6] = {
    uint3(0, 0, 0), // 0: -X (そのままでOK)
    uint3(1, 0, 0), // 1: +X (X軸を反転)
    uint3(0, 0, 0), // 2: -Y
    uint3(1, 0, 0), // 3: +Y
    uint3(0, 0, 0), // 4: -Z
    uint3(1, 0, 0)  // 5: +Z
};

float GetMappedDensity(int index, uint dirIndex, float3 basePos, float step, uint chunkID)
{
    // 1. 標準空間でのオフセット (0.0 ～ 1.0)
    float3 localOffset = kTransitionCornerOffsets[index];
    
    // 2. 方向に合わせて座標を回転
    float3 mappedOffset;
    mappedOffset[0] = localOffset[kPermutation[dirIndex][0]];
    mappedOffset[1] = localOffset[kPermutation[dirIndex][1]];
    mappedOffset[2] = localOffset[kPermutation[dirIndex][2]];

    // 3. 反転処理（復活）
    if (kInvert[dirIndex][0]) mappedOffset[0] = 1.0 - mappedOffset[0];
    if (kInvert[dirIndex][1]) mappedOffset[1] = 1.0 - mappedOffset[1];
    if (kInvert[dirIndex][2]) mappedOffset[2] = 1.0 - mappedOffset[2];
    
    // 4. ワールド座標に変換
    float32_t3 fstep3 = float32_t3(step, step, step);
    float3 samplePos = basePos + (mappedOffset * fstep3);
    
    return GetDensity(samplePos, chunkID);
}

VertexOut ProcessTransvoxelVertex(float3 worldPos, float3 chunkOrigin, uint chunkID) {
    VertexOut vOut;
    
    vOut.worldPosition = float4(worldPos + chunkOrigin, 1.0);
    vOut.position = mul(vOut.worldPosition, viewProjection.matVP);
    vOut.normal = CalculateNormal(worldPos, chunkID);
    // vOut.color = float4(1.0, 1.0, 1.0, 1.0); 
    vOut.color = DebugColor(chunkID);
    
    return vOut;
}



[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
    uint32_t3 DTid : SV_DispatchThreadID,
    in payload Payload payload,
    out vertices VertexOut verts[256],
    out indices uint32_t3 tris[256]) 
{
    uint32_t chunkID = payload.chunkID;
    float32_t3 chunkOrigin = payload.chunkOrigin;
    uint3 step = payload.subChunkSize;
    
    float3 basePos = float3(DTid * step);
    uint32_t3 localPos = DTid * step;
    uint32_t3 chunkSize = uint32_t3(voxelTerrainInfo.chunkSize);
    uint32_t transitionCode = 0;
    
    /// 境界面の判定
    bool isNX = (localPos.x == 0);
    bool isPX = (localPos.x >= chunkSize.x - step.x);
    bool isNY = (localPos.y == 0);
    bool isPY = (localPos.y >= chunkSize.y - step.y);
    bool isNZ = (localPos.z == 0);
    bool isPZ = (localPos.z >= chunkSize.z - step.z);
    
    // Transition方向の判定
    uint dirIndex = 0;
    
    if (isNX && (payload.transitionMask & TRANSITION_NX)) {
        transitionCode = 1;
        dirIndex = 0; // -X
    } else if (isPX && (payload.transitionMask & TRANSITION_PX)) {
        transitionCode = 2;
        dirIndex = 1; // +X
    } else if (isNZ && (payload.transitionMask & TRANSITION_NZ)) {
        transitionCode = 3;
        dirIndex = 4; // -Z
    } else if (isPZ && (payload.transitionMask & TRANSITION_PZ)) {
        transitionCode = 4;
        dirIndex = 5; // +Z
    }
    
    // 密度サンプリング
    float cellParams[13];
    uint caseCode = 0;
    if (transitionCode != 0) {
        // High-Res Face (0-8)
        for (int i = 0; i < 9; ++i) {
            float d = GetMappedDensity(i, dirIndex, basePos, step.x, chunkID);
            cellParams[i] = d;

            if (d >= voxelTerrainInfo.isoLevel) {
                caseCode |= (1u << i);
            }
        }
        // Low-Res Face (9-12)
        for (int j = 9; j < 13; ++j) {
            cellParams[j] = GetMappedDensity(j, dirIndex, basePos, step.x, chunkID);
        }
    }
    
    // テーブル参照
    uint classData      = 0;
    uint cellClass      = 0;
    uint geometryCounts = 0;
    uint vertexCount    = 0;
    uint triangleCount  = 0;

    // 空(0) または 完全に埋まっている(511) 場合は描画しない
    if (caseCode != 0 && caseCode != 511) {
    
        // テーブル参照
        classData = tTransitionCellClass[caseCode];
        cellClass = classData & 0x7F;
        geometryCounts = transitionCellData[cellClass].geometryCounts;

        vertexCount = (geometryCounts >> 4) & 0x0F;
        triangleCount = geometryCounts & 0x0F;
    }
    
    SetMeshOutputCounts(vertexCount, triangleCount);
    if (vertexCount == 0 || triangleCount == 0) {
        return;
    }
    
    // 頂点生成
    for (uint v = 0; v < vertexCount; ++v) {
        // データの直接参照（エッジテーブルは不要）
        uint vData = tTransitionVertexData[caseCode * kTransitionVertexDataStride + v];
        uint idxA = vData & 0x0F;
        uint idxB = (vData >> 4) & 0x0F;
        
        float valA = cellParams[idxA];
        float valB = cellParams[idxB];
        
        float3 posA = kTransitionCornerOffsets[idxA];
        float3 posB = kTransitionCornerOffsets[idxB];
        
        // 等値面補間
        float t = 0.5;
        float diff = valB - valA;
        if (abs(diff) > 1e-5) {
            t = (voxelTerrainInfo.isoLevel - valA) / diff;
        }
        t = saturate(t);
        
        float3 virtualPos = lerp(posA, posB, t);
        
        // 回転適用
        float3 mappedPos;
        mappedPos[0] = virtualPos[kPermutation[dirIndex][0]];
        mappedPos[1] = virtualPos[kPermutation[dirIndex][1]];
        mappedPos[2] = virtualPos[kPermutation[dirIndex][2]];
        
        // 反転処理（復活）
        if (kInvert[dirIndex][0]) mappedPos[0] = 1.0 - mappedPos[0];
        if (kInvert[dirIndex][1]) mappedPos[1] = 1.0 - mappedPos[1];
        if (kInvert[dirIndex][2]) mappedPos[2] = 1.0 - mappedPos[2];
        
        // ワールド座標に変換
        float32_t3 fstep = float32_t3(step.x, step.y, step.z);
        float3 worldPos = basePos + (mappedPos * fstep);
        
        verts[v] = ProcessTransvoxelVertex(worldPos, chunkOrigin, chunkID);
    }

    // インデックス生成
    for (uint t = 0; t < triangleCount; ++t) {
        uint i0 = transitionCellData[cellClass].vertexIndex[t * 3 + 0];
        uint i1 = transitionCellData[cellClass].vertexIndex[t * 3 + 1];
        uint i2 = transitionCellData[cellClass].vertexIndex[t * 3 + 2];
        
        tris[t] = uint3(i0, i1, i2);
    }
}