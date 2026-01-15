#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"

// -----------------------------------------------------------------------------
// Resources
// -----------------------------------------------------------------------------
Texture3D<float> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

static const float IsoLevel = 0.5f;
static const float GlobalVoxelSize = 1.0f;

// セルの頂点位置テーブル
static const int3 CornerOffsets[8] = {
    int3(0, 0, 0), int3(1, 0, 0), int3(1, 1, 0), int3(0, 1, 0),
    int3(0, 0, 1), int3(1, 0, 1), int3(1, 1, 1), int3(0, 1, 1)
};

// エッジの頂点接続
static const int2 EdgeConnection[12] = {
    int2(0,1), int2(1,2), int2(2,3), int2(3,0),
    int2(4,5), int2(5,6), int2(6,7), int2(7,4),
    int2(0,4), int2(1,5), int2(2,6), int2(3,7)
};

// チャンクIDから3Dグリッド位置を計算
uint3 GetChunkGridPos(uint chunkId)
{
    uint x = chunkId % voxelTerrainInfo.chunkCountXZ.x;
    uint z = (chunkId / voxelTerrainInfo.chunkCountXZ.x) % voxelTerrainInfo.chunkCountXZ.y;
    uint y = chunkId / (voxelTerrainInfo.chunkCountXZ.x * voxelTerrainInfo.chunkCountXZ.y);
    return uint3(x, y, z);
}

// チャンクIDからワールド位置を計算
float3 GetChunkWorldPosition(uint chunkId)
{
    uint3 gridPos = GetChunkGridPos(chunkId);
    return voxelTerrainInfo.terrainOrigin + float3(gridPos) * float3(voxelTerrainInfo.chunkSize) * GlobalVoxelSize;
}

// 隣接チャンクIDを取得 (方向: 0=X+, 1=X-, 2=Y+, 3=Y-, 4=Z+, 5=Z-)
int GetNeighborChunkId(uint chunkId, int direction)
{
    uint3 gridPos = GetChunkGridPos(chunkId);
    int3 offset = int3(0, 0, 0);
    
    if (direction == 0) offset = int3(1, 0, 0);       // X+
    else if (direction == 1) offset = int3(-1, 0, 0); // X-
    else if (direction == 2) offset = int3(0, 1, 0);  // Y+
    else if (direction == 3) offset = int3(0, -1, 0); // Y-
    else if (direction == 4) offset = int3(0, 0, 1);  // Z+
    else if (direction == 5) offset = int3(0, 0, -1); // Z-
    
    int3 neighborPos = int3(gridPos) + offset;
    
    // 境界チェック
    if (neighborPos.x < 0 || neighborPos.x >= (int)voxelTerrainInfo.chunkCountXZ.x ||
        neighborPos.z < 0 || neighborPos.z >= (int)voxelTerrainInfo.chunkCountXZ.y ||
        neighborPos.y < 0)
    {
        return -1; // 無効なチャンク
    }
    
    return neighborPos.x + neighborPos.z * voxelTerrainInfo.chunkCountXZ.x + 
           neighborPos.y * voxelTerrainInfo.chunkCountXZ.x * voxelTerrainInfo.chunkCountXZ.y;
}

// ボクセル値を取得 (チャンクIDとローカル座標から)
float GetVoxelValue(uint chunkId, float3 localPos)
{
    Chunk chunk = chunks[chunkId];
    float3 uvw = localPos / float3(voxelTerrainInfo.chunkSize);
    uvw = saturate(uvw);
    return voxelChunkTextures[chunk.textureId].SampleLevel(texSampler, uvw, 0);
}

// 隣接チャンクからボクセル値を取得 (境界処理用)
float GetVoxelValueWithNeighbor(uint chunkId, float3 localPos)
{
    float3 chunkSizeF = float3(voxelTerrainInfo.chunkSize);
    
    // チャンク内の場合
    if (all(localPos >= 0) && all(localPos < chunkSizeF))
    {
        return GetVoxelValue(chunkId, localPos);
    }
    
    // 境界外の場合、隣接チャンクから取得
    // X+ 方向
    if (localPos.x >= chunkSizeF.x)
    {
        int neighborId = GetNeighborChunkId(chunkId, 0);
        if (neighborId != -1)
        {
            float3 neighborPos = float3(localPos.x - chunkSizeF.x, localPos.y, localPos.z);
            return GetVoxelValue(neighborId, neighborPos);
        }
    }
    // X- 方向
    if (localPos.x < 0)
    {
        int neighborId = GetNeighborChunkId(chunkId, 1);
        if (neighborId != -1)
        {
            float3 neighborPos = float3(localPos.x + chunkSizeF.x, localPos.y, localPos.z);
            return GetVoxelValue(neighborId, neighborPos);
        }
    }
    // Y+ 方向
    if (localPos.y >= chunkSizeF.y)
    {
        int neighborId = GetNeighborChunkId(chunkId, 2);
        if (neighborId != -1)
        {
            float3 neighborPos = float3(localPos.x, localPos.y - chunkSizeF.y, localPos.z);
            return GetVoxelValue(neighborId, neighborPos);
        }
    }
    // Y- 方向
    if (localPos.y < 0)
    {
        int neighborId = GetNeighborChunkId(chunkId, 3);
        if (neighborId != -1)
        {
            float3 neighborPos = float3(localPos.x, localPos.y + chunkSizeF.y, localPos.z);
            return GetVoxelValue(neighborId, neighborPos);
        }
    }
    // Z+ 方向
    if (localPos.z >= chunkSizeF.z)
    {
        int neighborId = GetNeighborChunkId(chunkId, 4);
        if (neighborId != -1)
        {
            float3 neighborPos = float3(localPos.x, localPos.y, localPos.z - chunkSizeF.z);
            return GetVoxelValue(neighborId, neighborPos);
        }
    }
    // Z- 方向
    if (localPos.z < 0)
    {
        int neighborId = GetNeighborChunkId(chunkId, 5);
        if (neighborId != -1)
        {
            float3 neighborPos = float3(localPos.x, localPos.y, localPos.z + chunkSizeF.z);
            return GetVoxelValue(neighborId, neighborPos);
        }
    }
    
    return 0.0; // デフォルト値
}

// 法線を計算
float3 CalculateNormal(uint chunkId, float3 localPos)
{
    float delta = GlobalVoxelSize * 0.5;
    float3 grad;
    
    grad.x = GetVoxelValueWithNeighbor(chunkId, localPos + float3(delta, 0, 0)) - 
             GetVoxelValueWithNeighbor(chunkId, localPos - float3(delta, 0, 0));
    grad.y = GetVoxelValueWithNeighbor(chunkId, localPos + float3(0, delta, 0)) - 
             GetVoxelValueWithNeighbor(chunkId, localPos - float3(0, delta, 0));
    grad.z = GetVoxelValueWithNeighbor(chunkId, localPos + float3(0, 0, delta)) - 
             GetVoxelValueWithNeighbor(chunkId, localPos - float3(0, 0, delta));
    
    return normalize(-grad);
}

// エッジ上の交点を補間
float3 VertexInterpolate(float3 p1, float3 p2, float v1, float v2)
{
    if (abs(IsoLevel - v1) < 0.00001) return p1;
    if (abs(IsoLevel - v2) < 0.00001) return p2;
    if (abs(v1 - v2) < 0.00001) return p1;
    
    float t = (IsoLevel - v1) / (v2 - v1);
    return lerp(p1, p2, t);
}

// Transvoxelテーブルからバイトデータを抽出
uint ExtractByte(uint packedData, uint byteIndex)
{
    return (packedData >> (byteIndex * 8)) & 0xFF;
}


uint3 UnflattenIndex(uint index, uint3 dim)
{
    uint x = index % dim.x;
    uint y = (index / dim.x) % dim.y;
    uint z = index / (dim.x * dim.y);
    return uint3(x, y, z);
}

float3 GetChunkWorldOrigin(uint chunkId)
{
    uint x = chunkId % voxelTerrainInfo.chunkCountXZ.x;
    uint z = (chunkId / voxelTerrainInfo.chunkCountXZ.x) % voxelTerrainInfo.chunkCountXZ.y;
    uint y = chunkId / (voxelTerrainInfo.chunkCountXZ.x * voxelTerrainInfo.chunkCountXZ.y);
    return voxelTerrainInfo.terrainOrigin + float3(x, y, z) * float3(voxelTerrainInfo.chunkSize);
}

float SampleDensity(uint3 gridPos, uint stepSize, uint chunkIndex)
{
    return voxelChunkTextures[chunkIndex].Load(int4(gridPos * stepSize, 0));
}

VertexOut GetVertex(uint3 cellOrigin, uint stepSize, float3 p1, float3 p2, float v1, float v2, float3 chunkWorldOrigin)
{
    float t = (IsoLevel - v1) / (v2 - v1);
    float3 localP = p1 + (p2 - p1) * t;
    
    // 座標スケーリング： (セル位置 + 補間位置) * StepSize
    float3 posInChunk = (float3(cellOrigin) + localP) * float(stepSize);
    float3 worldPos = chunkWorldOrigin + posInChunk;
    
    VertexOut vOut;
    vOut.position = mul(float4(worldPos, 1.0), viewProjection.matVP);
    vOut.worldPosition = float4(worldPos, 1.0);
    vOut.normal = float3(0, 1, 0); 
    vOut.color = float4(1, 1, 1, 1);
    return vOut;
}

// グループ共有メモリ
groupshared float3 s_corners[8];
groupshared float s_values[8];
groupshared uint s_caseCode;
groupshared uint s_cellClass;
groupshared uint s_vertexCount;
groupshared uint s_triangleCount;
groupshared float3 s_vertices[12];


// Mesh Shader - セルごとにメッシュレットを生成
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
    uint gid : SV_GroupID,      // 今は何番目のグループ(セル)か
    in payload Payload payload,
    out vertices VertexOut verts[64],
    out indices uint3 tris[126]) {

    // 1. パラメータ展開
    uint chunkId = payload.chunkId;
    uint lod = payload.lodLevel;
    uint transitionMask = payload.transitionMask;
    uint stepSize = 1u << lod;

    uint3 rawSize = voxelTerrainInfo.chunkSize;
    uint3 effectiveDim = (rawSize - 1) / stepSize;
    
    // 【修正点】1スレッド1セルなので、グループIDがそのままセルインデックスになる
    uint cellIndex = gid;
    
    // 範囲外なら何もしない (ASで個数制御していれば基本起きないが念のため)
    if (cellIndex >= effectiveDim.x * effectiveDim.y * effectiveDim.z) return;

    uint3 cellPos = UnflattenIndex(cellIndex, effectiveDim);
    float3 chunkWorldOrigin = GetChunkWorldOrigin(chunkId);

    // 2. 密度値収集 (stepSize考慮はそのまま維持！)
    float corners[8];
    uint caseCode = 0;
    for(int i=0; i<8; ++i) {
        corners[i] = SampleDensity(cellPos + CornerOffsets[i], stepSize, chunkId);
        if(corners[i] >= IsoLevel) caseCode |= (1u << i);
    }

    if (caseCode == 0 || caseCode == 255) return;

    // 3. Transition判定 & データ数取得
    bool isTransition = false;
    uint transitionCode = 0;

    // 簡易判定 (前回と同じ)
    if ((cellPos.x == 0) && (transitionMask & 0x01)) { isTransition = true; transitionCode = (caseCode & 0xFF); }
    else if ((cellPos.x == effectiveDim.x - 1) && (transitionMask & 0x02)) { isTransition = true; transitionCode = (caseCode & 0xFF) | (1 << 8); }
    else if ((cellPos.z == 0) && (transitionMask & 0x10)) { isTransition = true; transitionCode = (caseCode & 0xFF) | (2 << 8); }
    else if ((cellPos.z == effectiveDim.z - 1) && (transitionMask & 0x20)) { isTransition = true; transitionCode = (caseCode & 0xFF) | (3 << 8); }

    uint vertexCount = 0;
    uint triangleCount = 0;
    uint cellClass = 0;

    // データ取得 (ここはテーブル仕様に合わせて調整が必要ですが、構造はシンプルに)
    if (isTransition) {
        // 本来は tTransitionCellClass 等を使用
        // 未実装なら Regular にフォールバック
        cellClass = tRegularCellClass[caseCode];
        vertexCount = (cellClass >> 8) & 0x0F;
        triangleCount = (cellClass >> 12) & 0x0F;
    } else {
        cellClass = tRegularCellClass[caseCode];
        vertexCount = (cellClass >> 8) & 0x0F;
        triangleCount = (cellClass >> 12) & 0x0F;
    }

    // 4. 出力設定 (Wave計算不要！)
    // 自分しかいないので、自分の出力数がそのままグループの出力数
    SetMeshOutputCounts(vertexCount, triangleCount);

    // 5. 書き込み (オフセット計算不要！)
    if (triangleCount > 0)
    {
        // A. 頂点書き込み (常にインデックス 0 から)
        for (uint i = 0; i < vertexCount; ++i)
        {
             // ※ tRegularVertexDataの仕様に合わせて読み出し
             // ここでは「12個のデータが並んでいる」と仮定したアクセスの例
             uint cellTypeIndex = (cellClass & 0xFF); 
             uint edgeCode = tRegularVertexData[cellTypeIndex * 12 + i]; 

             uint v1 = (edgeCode >> 0) & 0x0F;
             uint v2 = (edgeCode >> 4) & 0x0F;
             
             // 座標計算 (stepSize倍は必須！)
             VertexOut vOut = GetVertex(cellPos, stepSize, 
                                        float3(CornerOffsets[v1]), float3(CornerOffsets[v2]), 
                                        corners[v1], corners[v2], chunkWorldOrigin);
             
             verts[i] = vOut; // オフセットなし！
        }

        // B. インデックス書き込み (常にインデックス 0 から)
        for (uint t = 0; t < triangleCount; ++t)
        {
            uint cellTypeIndex = (cellClass & 0xFF);
            uint3 localIndices;
            // ※ tRegularCellDataの仕様に合わせて読み出し
            localIndices.x = tRegularCellData[cellTypeIndex * 15 + (t * 3) + 0];
            localIndices.y = tRegularCellData[cellTypeIndex * 15 + (t * 3) + 1];
            localIndices.z = tRegularCellData[cellTypeIndex * 15 + (t * 3) + 2];
            
            tris[t] = localIndices; // オフセットなし！
        }
    }
}