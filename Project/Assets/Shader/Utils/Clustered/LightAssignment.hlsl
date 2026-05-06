#include "ClusteredLight.hlsli"
#include "../../Schema/Schema.hlsli"

ConstantBuffer<ClusterParams> gParams : register(b0);
ConstantBuffer<SceneData> gSceneData : register(b1); // ビュー行列が必要
StructuredBuffer<ClusterAABB> gClusters : register(t0);
StructuredBuffer<PointLightData> gPointLights : register(t1);

RWStructuredBuffer<LightGrid> gLightGrid : register(u0);
RWStructuredBuffer<uint> gLightIndexList : register(u1);
RWStructuredBuffer<uint> gGlobalIndexCount : register(u2); // インデックスリストの現在位置

bool sphere_aabb_intersect(float3 center, float radius, ClusterAABB aabb) {
    float3 closest = max(aabb.minPoint.xyz, min(center, aabb.maxPoint.xyz));
    float distSq = dot(closest - center, closest - center);
    return distSq <= (radius * radius);
}

[numthreads(CLUSTER_GRID_X, CLUSTER_GRID_Y, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    // スレッド(0,0,0)が代表してカウンタをリセット
    if (all(DTid == 0)) {
        gGlobalIndexCount[0] = 0;
    }
    DeviceMemoryBarrierWithGroupSync(); // リセット完了まで待機

    uint clusterIdx = DTid.x + DTid.y * CLUSTER_GRID_X + DTid.z * CLUSTER_GRID_X * CLUSTER_GRID_Y;
    ClusterAABB aabb = gClusters[clusterIdx];

    uint clusterLightCount = 0;
    uint clusterLightIndices[MAX_LIGHTS_PER_CLUSTER];

    for (uint i = 0; i < gParams.totalLights; ++i) {
        PointLightData light = gPointLights[i];
        
        // ワールド空間からビュー空間へ変換
        float3 viewPos = mul(float4(light.position, 1.0f), gSceneData.view).xyz;
        
        if (sphere_aabb_intersect(viewPos, light.radius, aabb)) {
            if (clusterLightCount < MAX_LIGHTS_PER_CLUSTER) {
                clusterLightIndices[clusterLightCount] = i;
                clusterLightCount++;
            }
        }
    }

    // グローバルリストへの書き込み
    uint offset;
    InterlockedAdd(gGlobalIndexCount[0], clusterLightCount, offset);

    for (uint j = 0; j < clusterLightCount; ++j) {
        gLightIndexList[offset + j] = clusterLightIndices[j];
    }

    gLightGrid[clusterIdx].offset = offset;
    gLightGrid[clusterIdx].count = clusterLightCount;
}
