#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"
#include "../VoxelTerrainTest/Table.hlsli"

// -----------------------------------------------------------------------------
// Resources
// -----------------------------------------------------------------------------
Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

// -----------------------------------------------------------------------------
// Transvoxel Definitions (Corrected)
// -----------------------------------------------------------------------------

// Transvoxel Definitions (Corrected to Standard Z-Major)
// 重要：テーブルデータと合わせるため、0->1 で Z軸(横) が進む定義にします。
static const float3 kTransitionCornerOffsets[13] = 
{
    // --- 高解像度面 (x=0) ---
    // Z方向(横)を先に進める
    float3(0, 0,   0),   // 0: (min, min)
    float3(0, 0,   0.5), // 1: (min, mid) <- Zが進む
    float3(0, 0,   1),   // 2: (min, max)
    
    float3(0, 0.5, 0),   // 3: (mid, min) <- Yが進む
    float3(0, 0.5, 0.5), // 4: Center
    float3(0, 0.5, 1),   // 5
    
    float3(0, 1,   0),   // 6: (max, min)
    float3(0, 1,   0.5), // 7
    float3(0, 1,   1),   // 8

    // --- 低解像度面 (x=1) ---
    float3(1, 0, 0),     // 9:  (0,0)
    float3(1, 0, 1),     // 10: (0,1)     <- Zが進む
    float3(1, 1, 0),     // 11: (1,0)
    float3(1, 1, 1)      // 12: (1,1)
};

static const uint3 kPermutation[6] = {
    uint3(0, 1, 2), // 0: -X (Base)
    uint3(0, 1, 2), // 1: +X
    
    // Y-Upなので、这里的 Index 2,3 は Y軸方向への遷移を担当すべき
    // 厚み(X)を ワールドY に持ってくる設定
    uint3(2, 0, 1), // 2: -Y (修正: Thickness X -> World Y)
    uint3(2, 0, 1), // 3: +Y (修正: Thickness X -> World Y)
    
    // Index 4,5 は Z軸方向への遷移
    // 厚み(X)を ワールドZ に持ってくる設定
    uint3(1, 2, 0), // 4: -Z (修正: Thickness X -> World Z)
    uint3(1, 2, 0)  // 5: +Z (修正: Thickness X -> World Z)
};

// Permutation後のワールド軸(X,Y,Z)に対応する反転フラグ
static const uint3 kInvert[6] = {
    // 0: -X (厚みはX軸。Highを内側にしたい -> X反転)
    uint3(1, 0, 0), 
    // 1: +X (厚みはX軸。そのまま)
    uint3(0, 0, 0),
    // 2: -Y (厚みはY軸に来ている。Highを内側にしたい -> Y反転)
    uint3(0, 1, 0), // ★修正: (1,0,0) から (0,1,0) に変更
    // 3: +Y (厚みはY軸。そのまま)
    uint3(0, 0, 0),
    // 4: -Z (厚みはZ軸に来ている。Highを内側にしたい -> Z反転)
    uint3(0, 0, 1), // ★修正: (1,0,0) から (0,0,1) に変更
    // 5: +Z (厚みはZ軸。そのまま)
    uint3(0, 0, 0)
};


float4 GetColor(uint32_t seed)
{
    static const float3 colors[56] =
    {
        // --- Red ---
        float3(1,0,0), float3(0.8,0,0), float3(0.6,0,0),
        float3(1,0.4,0.4), float3(0.8,0.3,0.3),
        float3(0.6,0.2,0.2), float3(0.4,0.1,0.1),

        // --- Green ---
        float3(0,1,0), float3(0,0.8,0), float3(0,0.6,0),
        float3(0.4,1,0.4), float3(0.3,0.8,0.3),
        float3(0.2,0.6,0.2), float3(0.1,0.4,0.1),

        // --- Blue ---
        float3(0,0,1), float3(0,0,0.8), float3(0,0,0.6),
        float3(0.4,0.4,1), float3(0.3,0.3,0.8),
        float3(0.2,0.2,0.6), float3(0.1,0.1,0.4),

        // --- Yellow / Orange ---
        float3(1,1,0), float3(1,0.8,0), float3(1,0.6,0),
        float3(0.8,0.6,0), float3(0.8,0.8,0.4),
        float3(0.6,0.6,0.2), float3(0.4,0.4,0.1),

        // --- Magenta / Purple ---
        float3(1,0,1), float3(0.8,0,0.8), float3(0.6,0,0.6),
        float3(1,0.4,1), float3(0.8,0.3,0.8),
        float3(0.6,0.2,0.6), float3(0.4,0.1,0.4),

        // --- Cyan ---
        float3(0,1,1), float3(0,0.8,0.8), float3(0,0.6,0.6),
        float3(0.4,1,1), float3(0.3,0.8,0.8),
        float3(0.2,0.6,0.6), float3(0.1,0.4,0.4),

        // --- Gray ---
        float3(1,1,1), float3(0.8,0.8,0.8), float3(0.6,0.6,0.6),
        float3(0.4,0.4,0.4), float3(0.2,0.2,0.2),
        float3(0,0,0), float3(0.5,0.5,0.5),

        // --- Brown ---
        float3(0.6,0.3,0.1), float3(0.5,0.25,0.1),
        float3(0.4,0.2,0.1), float3(0.7,0.4,0.2),
        float3(0.5,0.35,0.2), float3(0.3,0.2,0.1),
        float3(0.2,0.15,0.1)
    };

    return float4(colors[seed % 56], 1.0);
}

float Encode3Bits(uint v)
{
    return v / 7.0; // 0～7 → 0～1
}

float4 CaseCodeToColor(uint caseCode)
{
    float r = Encode3Bits( caseCode        & 0x7);
    float g = Encode3Bits((caseCode >> 3) & 0x7);
    float b = Encode3Bits((caseCode >> 6) & 0x7);

    return float4(r, g, b, 1.0);
}

float3 TransoformOffset(float3 offset, uint dirIndex)
{
    float3 mappedOffset = offset;
    mappedOffset[0] = offset[kPermutation[dirIndex][0]];
    mappedOffset[1] = offset[kPermutation[dirIndex][1]];
    mappedOffset[2] = offset[kPermutation[dirIndex][2]];

    if (kInvert[dirIndex][0]) mappedOffset[0] = 1.0 - mappedOffset[0];
    if (kInvert[dirIndex][1]) mappedOffset[1] = 1.0 - mappedOffset[1];
    if (kInvert[dirIndex][2]) mappedOffset[2] = 1.0 - mappedOffset[2];

    return mappedOffset;
}

float4 Sampling(float3 samplePos, uint chunkID) {
    float voxelSize = 1.0f;
    float32_t3 textureSize = float32_t3(voxelTerrainInfo.textureSize);
    float32_t3 uvw = samplePos / (textureSize * voxelSize);
    return voxelChunkTextures[chunks[chunkID].textureId].SampleLevel(texSampler, uvw, 0);
}

// [解説] 3Dテクスチャから密度値(SDF等)をサンプリングする関数
float32_t GetDensity(float32_t3 samplePos, uint32_t chunkID) {
    float voxelSize = 1.0f;
    float32_t3 textureSize = float32_t3(voxelTerrainInfo.textureSize);
    float32_t3 uvw = (samplePos + 0.5f) / (textureSize * voxelSize);

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
        } else { return 1.0f; }
    } else if (uvw.x > 1.0f) {
        if (chunkX < int(voxelTerrainInfo.chunkCountXZ.x) - 1) {
            idx = chunkID + 1;
            uvw.x -= 1.0f;
        } else { return 1.0f; }
    }
    
    // Z方向の境界処理
    if (uvw.z < 0.0f) {
        if (chunkZ > 0) {
            idx -= uint(voxelTerrainInfo.chunkCountXZ.x);
            uvw.z += 1.0f;
        } else { return 1.0f; }
    } else if (uvw.z > 1.0f) {
        if (chunkZ < int(voxelTerrainInfo.chunkCountXZ.y) - 1) {
            idx += uint(voxelTerrainInfo.chunkCountXZ.x);
            uvw.z -= 1.0f;
        } else { return 1.0f; }
    }

    // Y方向の範囲外処理（空は空気、地下は固体）
    if (uvw.y < 0.0f) { return 1.0f; } // 地下
    if (uvw.y > 1.0f) { return 1.0f; } // 空

    uvw = saturate(uvw);

    float32_t4 rgba = voxelChunkTextures[chunks[idx].textureId].SampleLevel(texSampler, uvw, 0);
    return rgba.a;
}

float GetMappedDensity(int index, uint dirIndex, float3 basePos, float3 step, uint chunkID)
{
    float3 localOffset = kTransitionCornerOffsets[index];
    float3 samplePos = basePos + (TransoformOffset(localOffset, dirIndex) * step);
    return GetDensity(samplePos, chunkID);
}

VertexOut GetVertex(float3 p1, float3 p2, float3 chunkOrigin, float3 subChunkSize, float d1, float d2, uint dirIndex, uint id) {
    VertexOut vOut;
	
	// 補間係数の計算（ゼロ除算対策）
	float denom = d2 - d1;
	float t = 0.5f; // デフォルト値
	
    bool isDefault = false;
	if (abs(denom) > 0.00001f) {
		t = (voxelTerrainInfo.isoLevel - d1) / denom;
		t = saturate(t); // [0,1]にクランプ
        isDefault = true;
	}
	
	float3 localPos = lerp(p1, p2, t);
    
    // 回転適用
    float3 mappedPos = localPos;

	vOut.worldPosition = float4(mappedPos + chunkOrigin, 1.0f);
	vOut.position = mul(vOut.worldPosition, viewProjection.matVP);
    // float3 red = float3(1,0,0);
    // float3 blue = float3(0,0,1);
    // vOut.color = float4(lerp(red, blue, t), 1.0);
	// if(isDefault) {
    //     vOut.color = float4(0,1,0,1); // 補間成功は緑
    // }

    // vOut.color = CaseCodeToColor(id);
    
    return vOut;
}

float3 GetNormal(float3 p0, float3 p1, float3 p2) {
	float3 u = p1 - p0;
	float3 v = p2 - p0;
    float3 normal = cross(u, v);
    if(length(normal) < 1e-5) {
        return float3(0, 1, 0);
    }
    return normalize(normal);
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
    uint32_t3 chunkSize = uint32_t3(voxelTerrainInfo.chunkSize);
    uint32_t transitionCode = 0;
    
    /// ---------------------------------------------------
    /// 境界面の判定
    /// ---------------------------------------------------
    uint32_t3 localPos = basePos;
    bool isNX = (localPos.x == 0);
    bool isPX = (localPos.x >= chunkSize.x - step.x);
    bool isNY = (localPos.y == 0);
    bool isPY = (localPos.y >= chunkSize.y - step.y);
    bool isNZ = (localPos.z == 0);
    bool isPZ = (localPos.z >= chunkSize.z - step.z);
    
    // Transition方向の判定
    uint dirIndex = 0;
    
    if (isNX && (payload.transitionMask & TRANSITION_NX)) {
        // transitionCode = 1;
        // dirIndex = 0; // -X
    } else if (isPX && (payload.transitionMask & TRANSITION_PX)) {
        transitionCode = 2;
        dirIndex = 1; // +X
    } else if (isNZ && (payload.transitionMask & TRANSITION_NZ)) {
        // transitionCode = 3;
        // dirIndex = 4; // -Z
    } else if (isPZ && (payload.transitionMask & TRANSITION_PZ)) {
        // transitionCode = 4;
        // dirIndex = 5; // +Z
    }
    
    /// ---------------------------------------------------
    /// 密度サンプリング
    /// ---------------------------------------------------
    float cellParams[13];
    uint caseCode = 0;
    if (transitionCode != 0) {
        // High-Res Face (0-8)
        for (int i = 0; i < 9; ++i) {
            float d = GetMappedDensity(i, dirIndex, basePos, float32_t3(step), chunkID);
            cellParams[i] = d;

            if (d < voxelTerrainInfo.isoLevel) {
                caseCode |= (1u << i);
            }
        }
        // Low-Res Face (9-12)
        for (int j = 9; j < 13; ++j) {
            cellParams[j] = GetMappedDensity(j, dirIndex, basePos, float32_t3(step), chunkID);
        }
    }
    

    /// ---------------------------------------------------
    /// ジオメトリ計算
    /// ---------------------------------------------------
    uint classData      = 0;
    uint cellClass      = 0;
    uint geometryCounts = 0;
    uint vertexCount    = 0;
    uint triangleCount  = 0;

    // 空(0) または 完全に埋まっている(511) 場合は描画しない
    if (caseCode != 0 && caseCode != 511) {
        // テーブル参照
        classData = transitionCellClass[caseCode];
        cellClass = classData & 0x7F;
        geometryCounts = transitionCellData[cellClass][0];

        vertexCount = (geometryCounts >> 4) & 0x0F;
        triangleCount = geometryCounts & 0x0F;
    }
    
    GroupMemoryBarrier();
    SetMeshOutputCounts(vertexCount, triangleCount);
    if (vertexCount == 0 || triangleCount == 0) {
        return;
    }
    

    /// ---------------------------------------------------
    /// 頂点計算
    /// ---------------------------------------------------
    VertexOut vertexOuts[12];
    for (uint v = 0; v < vertexCount; ++v) {
        // データの直接参照（エッジテーブルは不要）
        uint vData = transitionVertexData[caseCode][v];
        uint idxA = vData & 0x0F;
        uint idxB = (vData >> 4) & 0x0F;
        
        float d1 = cellParams[idxA];
        float d2 = cellParams[idxB];
        
        float3 posA = TransoformOffset(kTransitionCornerOffsets[idxA], dirIndex);
        float3 posB = TransoformOffset(kTransitionCornerOffsets[idxB], dirIndex);
        
        // ワールド座標に変換
        float32_t3 p1 = basePos + (posA * float32_t3(step));
        float32_t3 p2 = basePos + (posB * float32_t3(step));
        
        // vertexOuts[v] = ProcessTransvoxelVertex(worldPos, chunkOrigin, chunkID, caseCode);
        vertexOuts[v] = GetVertex(p1, p2, chunkOrigin, float3(step), d1, d2, dirIndex, caseCode);
        vertexOuts[v].color = Sampling(basePos, chunkID);
    }

    /// ---------------------------------------------------
    /// インデックス計算
    /// ---------------------------------------------------
    for (uint t = 0; t < triangleCount; ++t) {
        uint i0 = transitionCellData[cellClass][t * 3 + 1];
        uint i1 = transitionCellData[cellClass][t * 3 + 2];
        uint i2 = transitionCellData[cellClass][t * 3 + 3];
        
        tris[t] = uint3(i0, i1, i2);
    }


    /// ---------------------------------------------------
    /// 法線の再計算
    /// ---------------------------------------------------
    for(uint32_t i = 0; i < triangleCount; ++i) {
        uint32_t3 tri = uint32_t3(
            transitionCellData[cellClass][i * 3 + 1],
            transitionCellData[cellClass][i * 3 + 2],
            transitionCellData[cellClass][i * 3 + 3]
        );

        float3 p0 = vertexOuts[tri.x].worldPosition.xyz;
        float3 p1 = vertexOuts[tri.y].worldPosition.xyz;
        float3 p2 = vertexOuts[tri.z].worldPosition.xyz;
        float3 normal = GetNormal(p0, p1, p2);
        vertexOuts[tri.x].normal = normal;
        vertexOuts[tri.y].normal = normal;
        vertexOuts[tri.z].normal = normal;
    }


    for(uint32_t v = 0; v < vertexCount; ++v) {
        verts[v] = vertexOuts[v];
    }
}