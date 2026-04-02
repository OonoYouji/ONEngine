#include "Transvoxel.hlsli"
#include "../../Texture.hlsli"

// -----------------------------------------------------------------------------
// Resources & Density
// -----------------------------------------------------------------------------
Texture3D<float4> voxelChunkTextures[kMaxTextureCount] : register(t1);
SamplerState texSampler : register(s0);

// 安全かつ正確な密度取得
float GetDensity(float3 worldPos) {
    float3 chunkSize = float3(voxelTerrainInfo.chunkSize);
    float3 textureSize = float3(voxelTerrainInfo.textureSize);
    
    int chunkX = floor(worldPos.x / chunkSize.x);
    int chunkZ = floor(worldPos.z / chunkSize.z);
    
    chunkX = clamp(chunkX, 0, (int)voxelTerrainInfo.chunkCountXZ.x - 1);
    chunkZ = clamp(chunkZ, 0, (int)voxelTerrainInfo.chunkCountXZ.y - 1);
    
    uint chunkId = chunkX + chunkZ * voxelTerrainInfo.chunkCountXZ.x;

    float3 currentChunkOrigin = float3(chunkX * chunkSize.x, 0, chunkZ * chunkSize.z);
    float3 uvw = (worldPos - currentChunkOrigin) / textureSize;
    uvw.y = 1.0 - uvw.y;
    
    uvw = saturate(uvw); 
    return voxelChunkTextures[chunks[chunkId].textureId].SampleLevel(texSampler, uvw, 0).a;
}

// -----------------------------------------------------------------------------
// Transvoxel Tables (最新のTransvoxel.ms.hlslと同期)
// -----------------------------------------------------------------------------
float GetMappedDensity(int index, uint dirIndex, float3 worldOrigin, float3 transitionScale) {
    float3 localOffset = kTransitionCornerOffsets[index];
    
    // 【修正1】オフセットが [-0.5, 0.5] のため、1.0から引くのではなく符号を反転する
    if (kInvert[dirIndex][0]) localOffset[0] = -localOffset[0];
    if (kInvert[dirIndex][1]) localOffset[1] = -localOffset[1];
    if (kInvert[dirIndex][2]) localOffset[2] = -localOffset[2];

    float3 mappedOffset;
    mappedOffset[0] = localOffset[kPermutation[dirIndex][0]];
    mappedOffset[1] = localOffset[kPermutation[dirIndex][1]];
    mappedOffset[2] = localOffset[kPermutation[dirIndex][2]];
    
    return GetDensity(worldOrigin + (mappedOffset * transitionScale));
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
    uint3 step = payload.subChunkSize;
    uint3 chunkSize = uint3(voxelTerrainInfo.chunkSize);
    
    // MCの中心基準と合わせるため、現在のボクセルの「左下奥(-0.5)」を起点とする
    float3 localPosBase = float3(DTid * step);
    float3 worldPosBase = payload.chunkOrigin + localPosBase;
    float3 worldOrigin = worldPosBase + float3(step) * 0.5f;
    
    // 面方向への2倍スケール
    float3 transitionScale = float3(step);
    
    // Transition Cell の判定
    uint3 localPos = uint3(localPosBase);
    bool isNX = (localPos.x == 0);
    bool isPX = (localPos.x >= chunkSize.x - step.x);
    bool isNY = (localPos.y == 0);
    bool isPY = (localPos.y >= chunkSize.y - step.y);
    bool isNZ = (localPos.z == 0);
    bool isPZ = (localPos.z >= chunkSize.z - step.z);
    
    uint transitionCode = 0;
    uint dirIndex = 0;
    if (isNX && (payload.transitionMask & TRANSITION_NX)) { transitionCode = 1; dirIndex = 0; }
    else if (isPX && (payload.transitionMask & TRANSITION_PX)) { transitionCode = 2; dirIndex = 1; }
    else if (isNY && (payload.transitionMask & TRANSITION_NY)) { transitionCode = 3; dirIndex = 2; }
    else if (isPY && (payload.transitionMask & TRANSITION_PY)) { transitionCode = 4; dirIndex = 3; }
    else if (isNZ && (payload.transitionMask & TRANSITION_NZ)) { transitionCode = 5; dirIndex = 4; }
    else if (isPZ && (payload.transitionMask & TRANSITION_PZ)) { transitionCode = 6; dirIndex = 5; }


    float cellParams[13];
    uint caseCode = 0;
    if (transitionCode != 0) {
        [unroll]
        for (int i = 0; i < 9; ++i) {
            float d = GetMappedDensity(i, dirIndex, worldOrigin, transitionScale);
            cellParams[i] = d;
            if (d < voxelTerrainInfo.isoLevel) caseCode |= (1u << i);
        }
        [unroll]
        for (int j = 9; j < 13; ++j) {
            float d = GetMappedDensity(j, dirIndex, worldOrigin, transitionScale);
            cellParams[j] = d;
        }
    }
    
    // -------------------------------------------------------------
    // 【デバッグ用】高解像度側(9個)のサンプリングポイントを可視化する処理
    // -------------------------------------------------------------
    
    // サンプリング点9個 × 箱1つ(8頂点・12ポリゴン) = 72頂点 / 108ポリゴン
    uint debugVertexCount = 0;
    uint debugTriangleCount = 0;
    
    if (transitionCode != 0) {
        debugVertexCount = 9 * 8;
        debugTriangleCount = 9 * 12;
    }

    GroupMemoryBarrierWithGroupSync();
    SetMeshOutputCounts(debugVertexCount, debugTriangleCount);
    if(debugVertexCount == 0 || debugTriangleCount == 0) return;
    
    
    float debugSize = step.x * 0.1f; // デバッグ用の箱のサイズ

    for (int i = 0; i < 9; ++i) {
        
        // 1. サンプリング位置の計算 (回転・反転の考慮)
        float3 localOffset = kTransitionCornerOffsets[i];

        // 【修正3】デバッグ側の反転も符号反転のみにする
        if (kInvert[dirIndex][0]) localOffset[0] = -localOffset[0];
        if (kInvert[dirIndex][1]) localOffset[1] = -localOffset[1];
        if (kInvert[dirIndex][2]) localOffset[2] = -localOffset[2];

        float3 mappedOffset;
        mappedOffset[0] = localOffset[kPermutation[dirIndex][0]];
        mappedOffset[1] = localOffset[kPermutation[dirIndex][1]];
        mappedOffset[2] = localOffset[kPermutation[dirIndex][2]];

        // worldOrigin は既にワールド座標（セル中心）になっているのでスケールを乗算して足すだけ
        float3 sampleWorldPos = worldOrigin + (mappedOffset * transitionScale);

        // 2. 密度の判定
        float d = cellParams[i];
        bool isSolid = (d < voxelTerrainInfo.isoLevel);

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
            vOut.worldPosition = float4(sampleWorldPos + (offsets[v] * debugSize), 1.0);
            vOut.position = mul(vOut.worldPosition, viewProjection.matVP);
            vOut.normal = normalize(offsets[v]);
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