#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"
#include "../../Texture.hlsli"

// -----------------------------------------------------------------------------
// Resources & Density
// -----------------------------------------------------------------------------
Texture3D<float4> voxelChunkTextures[kMaxTextureCount] : register(t1);
SamplerState texSampler : register(s0);

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

float GetMappedDensity(int index, uint dirIndex, float3 worldOrigin, float3 transitionScale) {
    float3 localOffset = kTransitionCornerOffsets[index];
    
    if (kInvert[dirIndex][0]) localOffset[0] = 1.0 - localOffset[0];
    if (kInvert[dirIndex][1]) localOffset[1] = 1.0 - localOffset[1];
    if (kInvert[dirIndex][2]) localOffset[2] = 1.0 - localOffset[2];

    float3 mappedOffset;
    mappedOffset[0] = localOffset[kPermutation[dirIndex][0]];
    mappedOffset[1] = localOffset[kPermutation[dirIndex][1]];
    mappedOffset[2] = localOffset[kPermutation[dirIndex][2]];
    
    return GetDensity(worldOrigin + (mappedOffset * transitionScale));
}


float3 CalculateNormal(float3 worldPos) {
    float delta = 0.01;
    float3 grad;
    grad.x = GetDensity(worldPos + float3(delta, 0, 0)) - GetDensity(worldPos - float3(delta, 0, 0));
    grad.y = GetDensity(worldPos + float3(0, delta, 0)) - GetDensity(worldPos - float3(0, delta, 0));
    grad.z = GetDensity(worldPos + float3(0, 0, delta)) - GetDensity(worldPos - float3(0, 0, delta));
    return normalize(-grad);
}


VertexOut ProcessTransvoxelVertex(float3 worldPos, float3 normal) {
    VertexOut vOut;
    vOut.worldPosition = float4(worldPos, 1.0);
    vOut.position = mul(vOut.worldPosition, viewProjection.matVP);
    vOut.normal = normal;
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
    uint3 step = payload.subChunkSize;
    uint3 chunkSize = uint3(voxelTerrainInfo.chunkSize);
    
    // MCの中心基準と合わせるため、現在のボクセルの「左下奥(-0.5)」を起点とする
    float3 localPosBase = float3(DTid * step);
    float3 worldPosBase = payload.chunkOrigin + localPosBase;
    float3 worldOrigin = worldPosBase + float3(step) * 0.5f;
    
    // Transition Cell のサンプリングでは、セルの境界から開始する（オフセットなし）
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

    // 面方向への2倍スケール
    float3 transitionScale = float3(step);
    if (transitionCode != 0) {
        if (dirIndex == 0 || dirIndex == 1) { transitionScale.y *= 2.0; transitionScale.z *= 2.0; }
        else if (dirIndex == 2 || dirIndex == 3) { transitionScale.x *= 2.0; transitionScale.z *= 2.0; }
        else if (dirIndex == 4 || dirIndex == 5) { transitionScale.x *= 2.0; transitionScale.y *= 2.0; }
    }

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

        // if(caseCode != 0 && caseCode != 511) {
        //     // デバッグ用に強制的にパラメータを上書きする場合のコード
        //     float debugCellParams[13] = {
        //         // [0-2] Y=0.0のライン (空気: +1.5)
        //         1.5,  1.5,  1.5,  
        //         // [3-5] Y=0.5のライン (空気: +0.5)
        //         0.5,  0.5,  0.5,  
        //         // [6-8] Y=1.0のライン (土: -0.5) ･･･ ここでビット「111」が立つ
        //         -0.5, -0.5, -0.5, 
        //         // [9-10] 奥側 Z=1.0, Y=0.0のライン (空気: +1.5)
        //         1.5,  1.5,        
        //         // [11-12] 奥側 Z=1.0, Y=1.0のライン (土: -0.5)
        //         -0.5, -0.5        
        //     };

        //     // 実際のcellParamsにコピー
        //     for (int d = 0; d < 13; ++d) {
        //         cellParams[d] = debugCellParams[d];
        //     }

        //     // 強制的にcaseCodeも448にする（念のため）
        //     caseCode = 448;
        // }

    }
    
    uint classData = 0, cellClass = 0, geometryCounts = 0, vertexCount = 0, triangleCount = 0;

    if (caseCode != 0 && caseCode != 511) {
        classData = tTransitionCellClass[caseCode];
        cellClass = classData & 0x7F;
        geometryCounts = transitionCellData[cellClass].geometryCounts;
        vertexCount = (geometryCounts >> 4) & 0x0F;
        triangleCount = geometryCounts & 0x0F;
    }
    
    GroupMemoryBarrierWithGroupSync();
    SetMeshOutputCounts(vertexCount, triangleCount);
    if (vertexCount == 0 || triangleCount == 0) return;
    
    // -------------------------------------------------------------
    // 頂点生成
    // -------------------------------------------------------------
    for (uint v = 0; v < vertexCount; ++v) {
        uint vData = tTransitionVertexData[caseCode * kTransitionVertexDataStride + v];
        
        uint idxA = (vData >> 0) & 0x0F;
        uint idxB = (vData >> 4) & 0x0F;
        
        float valA = cellParams[idxA];
        float valB = cellParams[idxB];
        float3 posA = kTransitionCornerOffsets[idxA];
        float3 posB = kTransitionCornerOffsets[idxB];
        
        float t = 0.0;
        float diff = valB - valA;
        if (abs(diff) > 1e-5) t = (voxelTerrainInfo.isoLevel - valA) / diff;
        t = saturate(t);
        
        float3 virtualPos = lerp(posA, posB, t);
        
        float3 physicalPos = virtualPos;
        if (kInvert[dirIndex][0]) physicalPos[0] = 1.0 - physicalPos[0];
        if (kInvert[dirIndex][1]) physicalPos[1] = 1.0 - physicalPos[1];
        if (kInvert[dirIndex][2]) physicalPos[2] = 1.0 - physicalPos[2];
        
        float3 mappedPos;
        mappedPos[0] = physicalPos[kPermutation[dirIndex][0]];
        mappedPos[1] = physicalPos[kPermutation[dirIndex][1]];
        mappedPos[2] = physicalPos[kPermutation[dirIndex][2]];
        
        float3 finalWorldPos = worldOrigin + (mappedPos * transitionScale);
        verts[v] = ProcessTransvoxelVertex(finalWorldPos, CalculateNormal(finalWorldPos));
    }

    // -------------------------------------------------------------
    // インデックス生成
    // -------------------------------------------------------------
    // bool invertClass = (classData & 0x80) != 0;
    // bool flipWinding = invertClass ^ kFlipWinding[dirIndex];

    for (uint t = 0; t < triangleCount; ++t) {
        uint i0 = transitionCellData[cellClass].vertexIndex[t * 3 + 0];
        uint i1 = transitionCellData[cellClass].vertexIndex[t * 3 + 1];
        uint i2 = transitionCellData[cellClass].vertexIndex[t * 3 + 2];
        
        // if (flipWinding) tris[t] = uint3(i0, i2, i1);
        // else 
        tris[t] = uint3(i0, i1, i2);
    }
}