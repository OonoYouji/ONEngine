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
// 隣接チャンクへの境界処理（境界を超えたら隣のテクスチャを読む処理）が含まれています
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
        } else {
            return 0.0f;
        }
    } else if (uvw.x > 1.0f) {
        if (chunkX < int(voxelTerrainInfo.chunkCountXZ.x) - 1) {
            idx = chunkID + 1;
            uvw.x -= 1.0f;
        } else {
            return 0.0f;
        }
    }
    
    // Z方向の境界処理
    if (uvw.z < 0.0f) {
        if (chunkZ > 0) {
            idx -= uint(voxelTerrainInfo.chunkCountXZ.x);
            uvw.z += 1.0f;
        } else {
            return 0.0f;
        }
    } else if (uvw.z > 1.0f) {
        if (chunkZ < int(voxelTerrainInfo.chunkCountXZ.y) - 1) {
            idx += uint(voxelTerrainInfo.chunkCountXZ.x);
            uvw.z -= 1.0f;
        } else {
            return 0.0f;
        }
    }

    if (uvw.y < 0.0f || uvw.y > 1.0f) {
        return 1.0f;
    }

    uvw = saturate(uvw);

    float32_t4 rgba = voxelChunkTextures[chunks[idx].textureId].SampleLevel(texSampler, uvw, 0);
    return rgba.a;
}

float3 GetDebugColor(uint ID) {
    uint r = (ID * 123456789u + 12345u) % 256u;
    uint g = (ID * 987654321u + 54321u) % 256u;
    uint b = (ID * 192837465u + 13579u) % 256u;
    return float3(r, g, b) / 255.0f;
}

uint32_t GetSubChunkSize(uint32_t lodLevel) {
    return 2u << (lodLevel);
}


float3 GetNormal(float3 _p0, float3 _p1, float3 _p2) {
    float3 u = _p1 - _p0;
    float3 v = _p2 - _p0;
    return normalize(cross(u, v));
}
// Transvoxel.ms.hlsl 修正版

static const float3 kTransitionCornerOffsets[13] = {
    float3(0.0, 0.0, 0.0), // 0
    float3(0.0, 1.0, 0.0), // 1
    float3(0.0, 1.0, 1.0), // 2
    float3(0.0, 0.0, 1.0), // 3
    
    float3(0.0, 0.5, 0.0), // 4  エッジ中点
    float3(0.0, 1.0, 0.5), // 5
    float3(0.0, 0.5, 1.0), // 6
    float3(0.0, 0.0, 0.5), // 7
    
    float3(0.0, 0.5, 0.5), // 8  面中心
    
    float3(1.0, 0.0, 0.0), // 9  高解像度側
    float3(1.0, 1.0, 0.0), // 10
    float3(1.0, 1.0, 1.0), // 11
    float3(1.0, 0.0, 1.0)  // 12
};

// 回転テーブル（Transvoxel標準）
static const uint3 kPermutation[6] = {
    uint3(0, 1, 2), // 0: -X
    uint3(0, 1, 2), // 1: +X
    uint3(1, 0, 2), // 2: -Y
    uint3(1, 0, 2), // 3: +Y
    uint3(2, 1, 0), // 4: -Z
    uint3(2, 1, 0)  // 5: +Z
};

static const uint3 kInvert[6] = {
    uint3(0, 0, 0), // 0: -X
    uint3(1, 0, 0), // 1: +X
    uint3(0, 0, 0), // 2: -Y
    uint3(1, 0, 0), // 3: +Y
    uint3(0, 0, 0), // 4: -Z
    uint3(1, 0, 0)  // 5: +Z
};

// 修正版: 密度取得（回転考慮）
float GetMappedDensity(int index, uint dirIndex, float3 basePos, float step, uint chunkID)
{
    // 1. 標準空間でのオフセット (0.0 ～ 1.0)
    float3 localOffset = kTransitionCornerOffsets[index];
    
    // 2. 方向に合わせて座標を回転
    float3 mappedOffset;
    mappedOffset[0] = localOffset[kPermutation[dirIndex][0]];
    mappedOffset[1] = localOffset[kPermutation[dirIndex][1]];
    mappedOffset[2] = localOffset[kPermutation[dirIndex][2]];
    
    // 3. 反転（0-1座標系では 1.0 - x）
    if (kInvert[dirIndex][0]) mappedOffset[0] = 1.0 - mappedOffset[0];
    if (kInvert[dirIndex][1]) mappedOffset[1] = 1.0 - mappedOffset[1];
    if (kInvert[dirIndex][2]) mappedOffset[2] = 1.0 - mappedOffset[2];
    
    // 4. ワールド座標に変換
    // basePos はボクセルの「左下角」を指すように修正
    float32_t3 fstep3 = float32_t3(step, step, step);
    float3 samplePos = basePos + (mappedOffset * fstep3);
    
    return GetDensity(samplePos, chunkID);
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

// 修正版: 頂点処理
VertexOut ProcessTransvoxelVertex(float3 worldPos, float3 chunkOrigin, uint chunkID) {
    VertexOut vOut;
    
    vOut.worldPosition = float4(worldPos + chunkOrigin, 1.0);
    vOut.position = mul(vOut.worldPosition, viewProjection.matVP);
    
    // 法線計算
    vOut.normal = CalculateNormal(worldPos, chunkID);
    
    // デバッグ用カラー（TransitionCellを赤色で表示）
    vOut.color = float4(1.0, 0.0, 0.0, 1.0);
    
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
    
    // ★重要: basePosをボクセルの「左下角」座標に修正
    float3 basePos = float3(DTid * step);
    uint32_t3 localPos = DTid * step;
    uint32_t3 chunkSize = uint32_t3(voxelTerrainInfo.chunkSize);
    uint32_t transitionCode = 0;
    
    // ★修正: より正確な境界チェック
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
    }
    else if (isPX && (payload.transitionMask & TRANSITION_PX)) {
        transitionCode = 2;
        dirIndex = 1; // +X
    }
    else if (isNZ && (payload.transitionMask & TRANSITION_NZ)) {
        transitionCode = 3;
        dirIndex = 4; // -Z
    }
    else if (isPZ && (payload.transitionMask & TRANSITION_PZ)) {
        transitionCode = 4;
        dirIndex = 5; // +Z
    }
    
    // Transitionが不要な場合は早期リターン

    
    // 密度サンプリング
    float cellParams[13];
    uint caseCode = 0;
    if (transitionCode != 0) {
        for (int i = 0; i < 13; ++i) {
            float d = GetMappedDensity(i, dirIndex, basePos, step.x, chunkID);
            cellParams[i] = d;

            if (i < 9 && d < voxelTerrainInfo.isoLevel) {
                caseCode |= (1u << i);
            }
        }
    }
    

    // テーブル参照
    uint classData      = 0;
    uint cellClass      = 0;
    uint baseIndex      = 0;
    uint geometryCounts = 0;
    uint vertexCount    = 0;
    uint triangleCount  = 0;

    // 空または完全に埋まっている場合
    if (caseCode != 0 && caseCode != 511) {
    
        // テーブル参照
        classData = tTransitionCellClass[caseCode];
        cellClass = classData & 0x7F;
        baseIndex = cellClass * kTransitionCellDataStride;
        geometryCounts = tTransitionCellData[baseIndex];

        vertexCount = (geometryCounts >> 4) & 0x0F;
        triangleCount = geometryCounts & 0x0F;
    }
    

    // GroupMemoryBarrierWithGroupSync();
    SetMeshOutputCounts(vertexCount, triangleCount);
    if (vertexCount == 0 || triangleCount == 0) {
        return;
    }
    
    // 頂点生成
    for (uint v = 0; v < vertexCount; ++v) {
        uint vData = tTransitionVertexData[cellClass * kTransitionVertexDataStride + v];
        uint idxA = vData & 0x0F;
        uint idxB = (vData >> 4) & 0x0F;
        
        float valA = cellParams[idxA];
        float valB = cellParams[idxB];
        
        // 標準空間でのオフセット (0-1)
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
        
        // 反転
        if (kInvert[dirIndex][0]) mappedPos[0] = 1.0 - mappedPos[0];
        if (kInvert[dirIndex][1]) mappedPos[1] = 1.0 - mappedPos[1];
        if (kInvert[dirIndex][2]) mappedPos[2] = 1.0 - mappedPos[2];
        
        // ワールド座標に変換
        float32_t3 fstep = float32_t3(step.x, step.y, step.z);
        float3 worldPos = basePos + (mappedPos * fstep);
        
        verts[v] = ProcessTransvoxelVertex(worldPos, chunkOrigin, chunkID);
    }
    
    // インデックス生成（安全装置付き）
    for (uint t = 0; t < triangleCount; ++t) {
        uint i0 = tTransitionCellData[baseIndex + 1 + (t * 3 + 0)];
        uint i1 = tTransitionCellData[baseIndex + 1 + (t * 3 + 1)];
        uint i2 = tTransitionCellData[baseIndex + 1 + (t * 3 + 2)];
        
        // クランプ（安全装置）
        i0 = min(i0, vertexCount - 1);
        i1 = min(i1, vertexCount - 1);
        i2 = min(i2, vertexCount - 1);

        bool flipWinding =
            (dirIndex == 1) || // +X
            (dirIndex == 3) || // +Y
            (dirIndex == 5);   // +Z

        if (flipWinding) {
            tris[t] = uint3(i0, i2, i1);
        } else {
            tris[t] = uint3(i0, i1, i2);
        }

    }
}