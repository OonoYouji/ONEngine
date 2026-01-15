// VoxelTerrainAS.hlsl
#include "Transvoxel.hlsli"

//--------------------------------------
// LOD 計算
//--------------------------------------
uint CalculateLOD(float3 worldPos, float3 cameraPos)
{
    float dist = distance(worldPos, cameraPos);

    if (dist < 50.0f)  return 0; // finest
    if (dist < 100.0f) return 1;
    if (dist < 200.0f) return 2;
    return 3;                   // coarsest
}

uint GetStepSize(uint lod)
{
    // Transvoxel 前提：LOD が 1 上がるごとに 2 倍
    return 1u << (lod + 1); // 2,4,8,16
}

//--------------------------------------
// Chunk index helper
//--------------------------------------
uint IndexOfMeshGroup(uint3 groupID, uint2 dimXZ)
{
    return groupID.x
         + groupID.z * dimXZ.x;
}

//--------------------------------------
// Amplification Shader
//--------------------------------------
[shader("amplification")]
[numthreads(1, 1, 1)]
void main(uint3 groupId : SV_GroupID)
{
    Payload payload;

    //----------------------------------
    // 1. Chunk 基本情報
    //----------------------------------
    payload.chunkIndex =
        IndexOfMeshGroup(
            groupId,
            voxelTerrainInfo.chunkCountXZ
        );

    payload.chunkOrigin =
        float3(groupId) * float3(voxelTerrainInfo.chunkSize)
        + voxelTerrainInfo.terrainOrigin;

    float3 chunkCenter =
        payload.chunkOrigin +
        float3(voxelTerrainInfo.chunkSize) * 0.5f;

    payload.lodLevel =
        CalculateLOD(chunkCenter, camera.position.xyz);

    uint stepSize = GetStepSize(payload.lodLevel);
    payload.subChunkSize = uint3(stepSize, stepSize, stepSize);

    // 1 辺あたりのセル数
    payload.faceResolution =
        voxelTerrainInfo.chunkSize.x / stepSize;

    //----------------------------------
    // 2. Transvoxel 遷移マスク生成
    //----------------------------------
    payload.transitionMask = 0;

    float3 size = float3(voxelTerrainInfo.chunkSize);

    // Transvoxel face bit 定義（MS 側と完全一致させる）
    // bit:
    // 0 : -X
    // 1 : +X
    // 4 : -Z
    // 5 : +Z
    struct Neighbor
    {
        float3 offset;
        uint   bit;
    };

    Neighbor neighbors[4] =
    {
        { float3(-size.x, 0, 0), 0 },
        { float3( size.x, 0, 0), 1 },
        { float3( 0, 0,-size.z), 4 },
        { float3( 0, 0, size.z), 5 }
    };

    uint activeFaceCount = 0;

    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        float3 neighborCenter =
            chunkCenter + neighbors[i].offset;

        uint neighborLOD =
            CalculateLOD(neighborCenter, camera.position.xyz);

        // 隣が粗い → 自分が高解像度 → Transvoxel 適用
        if (neighborLOD > payload.lodLevel)
        {
            payload.transitionMask |= (1u << neighbors[i].bit);
            activeFaceCount++;
        }
    }

    //----------------------------------
    // 3. DispatchMesh
    //----------------------------------
    // faceResolution^2 スレッド / face
    uint threadsPerFace =
        payload.faceResolution *
        payload.faceResolution;

    uint totalThreads =
        activeFaceCount * threadsPerFace;

    uint3 dispatchSize = uint3(totalThreads, 1, 1);

    // Transvoxel face が無い場合は Dispatch しない
    if (totalThreads > 0)
    {
        dispatchSize = uint3(0,0,0);
    }


    DispatchMesh(dispatchSize.x, dispatchSize.y, dispatchSize.z, payload);
}
