#include "ClusteredLight.hlsli"

ConstantBuffer<ClusterParams> gParams : register(b0);
RWStructuredBuffer<ClusterAABB> gClusters : register(u0);

float4 screen_to_view(float4 screen) {
    float2 texCoord = screen.xy / float2(gParams.screenWidth, gParams.screenHeight);
    float4 clip = float4(texCoord.x * 2.0f - 1.0f, (1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);
    float4 view = mul(clip, gParams.invProj);
    return view / view.w;
}

float3 intersect_line_z_plane(float3 a, float3 b, float z) {
    float3 ab = b - a;
    float t = (z - a.z) / ab.z;
    return a + t * ab;
}

[numthreads(CLUSTER_GRID_X, CLUSTER_GRID_Y, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint clusterIdx = DTid.x + DTid.y * CLUSTER_GRID_X + DTid.z * CLUSTER_GRID_X * CLUSTER_GRID_Y;
    
    // クラスタの4角（スクリーン空間）
    float px = (float)DTid.x * (gParams.screenWidth / CLUSTER_GRID_X);
    float py = (float)DTid.y * (gParams.screenHeight / CLUSTER_GRID_Y);
    float pNextX = (float)(DTid.x + 1) * (gParams.screenWidth / CLUSTER_GRID_X);
    float pNextY = (float)(DTid.y + 1) * (gParams.screenHeight / CLUSTER_GRID_Y);

    // Z面（指数分割）
    float nearZ = gParams.nearZ * pow(abs(gParams.farZ / gParams.nearZ), (float)DTid.z / CLUSTER_GRID_Z);
    float farZ = gParams.nearZ * pow(abs(gParams.farZ / gParams.nearZ), (float)(DTid.z + 1) / CLUSTER_GRID_Z);

    // ビュー空間への変換
    float3 pMin = screen_to_view(float4(px, py, 0.0f, 1.0f)).xyz;
    float3 pMax = screen_to_view(float4(pNextX, pNextY, 0.0f, 1.0f)).xyz;

    // 視錐台の側面と Z 面の交点を求めて AABB を作成
    float3 minPoint = float3(10000, 10000, 10000);
    float3 maxPoint = float3(-10000, -10000, -10000);

    float3 corners[8];
    // 近平面
    corners[0] = intersect_line_z_plane(float3(0,0,0), pMin, nearZ);
    corners[1] = intersect_line_z_plane(float3(0,0,0), float3(pMax.x, pMin.y, pMin.z), nearZ);
    corners[2] = intersect_line_z_plane(float3(0,0,0), float3(pMin.x, pMax.y, pMin.z), nearZ);
    corners[3] = intersect_line_z_plane(float3(0,0,0), pMax, nearZ);
    // 遠平面
    corners[4] = intersect_line_z_plane(float3(0,0,0), pMin, farZ);
    corners[5] = intersect_line_z_plane(float3(0,0,0), float3(pMax.x, pMin.y, pMin.z), farZ);
    corners[6] = intersect_line_z_plane(float3(0,0,0), float3(pMin.x, pMax.y, pMin.z), farZ);
    corners[7] = intersect_line_z_plane(float3(0,0,0), pMax, farZ);

    for (int i = 0; i < 8; ++i) {
        minPoint = min(minPoint, corners[i]);
        maxPoint = max(maxPoint, corners[i]);
    }

    gClusters[clusterIdx].minPoint = float4(minPoint, 0.0f);
    gClusters[clusterIdx].maxPoint = float4(maxPoint, 0.0f);
}
