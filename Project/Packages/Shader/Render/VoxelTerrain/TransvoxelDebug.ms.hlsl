#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"
#include "../VoxelTerrainTest/Table.hlsli"

// -----------------------------------------------------------------------------
// Resources
// -----------------------------------------------------------------------------
Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

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
    if (uvw.y < 0.0f) { return 1.0f; } // 空
    if (uvw.y > 1.0f) { return 0.0f; } // 地下

    uvw = saturate(uvw);

    float32_t4 rgba = voxelChunkTextures[chunks[idx].textureId].SampleLevel(texSampler, uvw, 0);
    return rgba.a;
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
    uint3(0, 1, 2), // 0: -X (Thickness on X)
    uint3(0, 1, 2), // 1: +X
    uint3(2, 0, 1), // 2: -Y (mapped.y に local.x[厚み] を入れる)
    uint3(2, 0, 1), // 3: +Y
    uint3(1, 2, 0), // 4: -Z (mapped.z に local.x[厚み] を入れる)
    uint3(1, 2, 0)  // 5: +Z
};

// 反転テーブル（重要：プラス方向の面は高解像度面を外側に向けるため反転が必要）
static const uint3 kInvert[6] = {
    uint3(1, 0, 0), // 0: -X
    uint3(0, 0, 0), // 1: +X
    uint3(1, 0, 0), // 2: -Y
    uint3(0, 0, 0), // 3: +Y
    uint3(1, 0, 0), // 4: -Z
    uint3(0, 0, 0)  // 5: +Z
};

// float GetMappedDensity(int index, uint dirIndex, float3 basePos, float step, uint chunkID)
// {
//     // 1. 標準空間でのオフセット (0.0 ～ 1.0)
//     float3 localOffset = kTransitionCornerOffsets[index];
    
//     // 2. 方向に合わせて座標を回転
//     float3 mappedOffset = localOffset;
//     mappedOffset[0] = localOffset[kPermutation[dirIndex][0]];
//     mappedOffset[1] = localOffset[kPermutation[dirIndex][1]];
//     mappedOffset[2] = localOffset[kPermutation[dirIndex][2]];

//     // 3. 反転処理
//     if (kInvert[dirIndex][0]) mappedOffset[0] = 1.0 - mappedOffset[0];
//     if (kInvert[dirIndex][1]) mappedOffset[1] = 1.0 - mappedOffset[1];
//     if (kInvert[dirIndex][2]) mappedOffset[2] = 1.0 - mappedOffset[2];
    
//     // 4. ワールド座標に変換
//     float32_t3 fstep3 = float32_t3(step, step, step);
//     float3 samplePos = basePos + (mappedOffset * fstep3);
    
//     return GetDensity(samplePos, chunkID);
// }

float GetMappedDensity(int index, uint dirIndex, float3 basePos, float step, uint chunkID)
{
    float3 localOffset = kTransitionCornerOffsets[index];
    
    if (kInvert[dirIndex][0]) localOffset[0] = 1.0 - localOffset[0];
    if (kInvert[dirIndex][1]) localOffset[1] = 1.0 - localOffset[1];
    if (kInvert[dirIndex][2]) localOffset[2] = 1.0 - localOffset[2];

    // 3. 方向に合わせて座標を回転 (Canonical -> Physical)
    float3 mappedOffset;
    mappedOffset[0] = localOffset[kPermutation[dirIndex][0]];
    mappedOffset[1] = localOffset[kPermutation[dirIndex][1]];
    mappedOffset[2] = localOffset[kPermutation[dirIndex][2]];
    
    // 4. ワールド座標に変換
    float32_t3 fstep3 = float32_t3(step, step, step);
    float3 samplePos = basePos + (mappedOffset * fstep3);
    
    return GetDensity(samplePos, chunkID);
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
    uint32_t    chunkID     = payload.chunkID;
    float32_t3  chunkOrigin = payload.chunkOrigin;
    uint32_t3   chunkSize   = uint32_t3(voxelTerrainInfo.chunkSize);

    uint3       step        = payload.subChunkSize;
    float3      basePos     = float32_t3(DTid * step);
    uint32_t3   localPos    = uint32_t3(basePos);

    /// 境界面の判定
    bool isNX = (localPos.x == 0);
    bool isPX = (localPos.x >= chunkSize.x - step.x);
    bool isNY = (localPos.y == 0);
    bool isPY = (localPos.y >= chunkSize.y - step.y);
    bool isNZ = (localPos.z == 0);
    bool isPZ = (localPos.z >= chunkSize.z - step.z);
    
    // Transition方向の判定
    uint32_t transitionCode = 0;
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
    

    /// 密度サンプリング
    float cellParams[13];
    uint caseCode = 0;
    if (transitionCode != 0) {
        // High-Res Face (0-8)
        [unroll]
        for (int i = 0; i < 9; ++i) {
            float d = GetMappedDensity(i, dirIndex, basePos, step.x, chunkID);
            cellParams[i] = d;

            if (d < voxelTerrainInfo.isoLevel) {
                caseCode |= (1u << i);
            }
        }
        // Low-Res Face (9-12)
        [unroll]
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
    
    // -------------------------------------------------------------
    // 【デバッグ用】13個のサンプリングポイントを可視化する処理
    // -------------------------------------------------------------
    
    // サンプリング点13個 × 三角形1つ(3頂点) = 39頂点 / 13ポリゴン
    uint debugVertexCount = 0;
    uint debugTriangleCount = 0;
    if (caseCode != 0 && caseCode != 511) {
        debugVertexCount = 9 * 8;    // 72頂点
        debugTriangleCount = 9 * 12; // 108ポリゴン
    }

    GroupMemoryBarrierWithGroupSync();
    SetMeshOutputCounts(debugVertexCount, debugTriangleCount);

    if (transitionCode != 0) {
        float3 fstep = float3(step.x, step.y, step.z);
        // デバッグ用の箱のサイズ（大きすぎる/小さすぎる場合はここを調整）
        float debugSize = step.x * 0.1f; 

        for (int i = 0; i < 9; ++i) {
            
            // 1. サンプリング位置の計算 (回転・反転の考慮)
            float3 localOffset = kTransitionCornerOffsets[i];

            if (kInvert[dirIndex][0]) localOffset[0] = 1.0 - localOffset[0];
            if (kInvert[dirIndex][1]) localOffset[1] = 1.0 - localOffset[1];
            if (kInvert[dirIndex][2]) localOffset[2] = 1.0 - localOffset[2];

            float3 mappedOffset;
            mappedOffset[0] = localOffset[kPermutation[dirIndex][0]];
            mappedOffset[1] = localOffset[kPermutation[dirIndex][1]];
            mappedOffset[2] = localOffset[kPermutation[dirIndex][2]];

            // basePos は既にワールド座標のはずなので、そのままオフセットを足すだけ
            float3 sampleWorldPos = basePos + (mappedOffset * fstep);

            // 2. 密度の判定
            float d = cellParams[i];
            bool isSolid = (d < voxelTerrainInfo.isoLevel); // ★MC法と同じ条件

            // 固体なら赤、空気なら青
            float4 ptColor = isSolid ? float4(1.0, 0.0, 0.0, 1.0) : float4(0.0, 0.0, 1.0, 1.0);

            // 3. 小さな箱(Cube)の頂点を8つ生成
            uint vBase = i * 8;
            float3 offsets[8] = {
                float3(-1, -1, -1), float3( 1, -1, -1), float3( 1,  1, -1), float3(-1,  1, -1),
                float3(-1, -1,  1), float3( 1, -1,  1), float3( 1,  1,  1), float3(-1,  1,  1)
            };

            for(int v = 0; v < 8; ++v) {
                VertexOut vOut;
                // ワールド座標に直接オフセットを足す（chunkOriginは足さない！）
                vOut.worldPosition = float4(chunkOrigin + sampleWorldPos + (offsets[v] * debugSize), 1.0);
                vOut.position = mul(vOut.worldPosition, viewProjection.matVP);
                vOut.normal = normalize(offsets[v]); // 法線も適当に散らす
                vOut.color = ptColor;
                verts[vBase + v] = vOut;
            }

            // 4. 箱のインデックス(12ポリゴン)を生成
            uint tBase = i * 12;
            
            // 前面・背面
            tris[tBase + 0] = uint3(vBase+0, vBase+1, vBase+2); tris[tBase + 1] = uint3(vBase+0, vBase+2, vBase+3);
            tris[tBase + 2] = uint3(vBase+5, vBase+4, vBase+7); tris[tBase + 3] = uint3(vBase+5, vBase+7, vBase+6);
            // 左面・右面
            tris[tBase + 4] = uint3(vBase+4, vBase+0, vBase+3); tris[tBase + 5] = uint3(vBase+4, vBase+3, vBase+7);
            tris[tBase + 6] = uint3(vBase+1, vBase+5, vBase+6); tris[tBase + 7] = uint3(vBase+1, vBase+6, vBase+2);
            // 上面・下面
            tris[tBase + 8] = uint3(vBase+3, vBase+2, vBase+6); tris[tBase + 9] = uint3(vBase+3, vBase+6, vBase+7);
            tris[tBase + 10]= uint3(vBase+4, vBase+5, vBase+1); tris[tBase + 11]= uint3(vBase+4, vBase+1, vBase+0);
        }
    }

}