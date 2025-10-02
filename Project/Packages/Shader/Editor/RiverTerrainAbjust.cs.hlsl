#include "River.hlsli"
#include "Terrain.hlsli"

/// buffer
ConstantBuffer<RiverParams> params : register(b0);
RWStructuredBuffer<TerrainVertex> terrainVertices : register(u0);
RWStructuredBuffer<RiverVertex> riverVertices : register(u1);
RWStructuredBuffer<uint> riverIndices : register(u2);

float2 closestPointOnTriangle(float2 _p, float2 _a, float2 _b, float2 _c) {
    // 2D三角形の最近接点を計算
    float2 closest = _a; 
    float minDist2 = length(_p - _a);

    float2 ab = _b - _a;
    float t = saturate(dot(_p - _a, ab) / dot(ab, ab));
    float2 proj = _a + t * ab;
    float d2 = length(_p - proj);
    if(d2 < minDist2) { minDist2 = d2; closest = proj; }

    float2 bc = _c - _b;
    t = saturate(dot(_p - _b, bc) / dot(bc, bc));
    proj = _b + t * bc;
    d2 = length(_p - proj);
    if(d2 < minDist2) { minDist2 = d2; closest = proj; }

    float2 ca = _a - _c;
    t = saturate(dot(_p - _c, ca) / dot(ca, ca));
    proj = _c + t * ca;
    d2 = length(_p - proj);
    if(d2 < minDist2) { minDist2 = d2; closest = proj; }

    return closest;
}

[numthreads(32, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    /// 地形の頂点index
    uint tvIndex = DTid.x;
    float maxFactor = 0.0f;
    float chosenHeight = terrainVertices[tvIndex].position.y; // デフォルトは現在の高さ
    float falloffRadius = 0.1f; /// paramsに入れるように変更する
     
    /// 地形の頂点の座標を取得
    float3 tvPos = terrainVertices[tvIndex].position.xyz;

    int totalRiverTriangles = params.totalVertices * 6 / 2 - 6;
    for(int i = 0; i < totalRiverTriangles; ++i) {
        uint3 tri = uint3(
            riverIndices[i * 3+ 0],
            riverIndices[i * 3+ 1],
            riverIndices[i * 3+ 2]
        );

        /// x,z座標で三角形を構築
        float2 a = float2(riverVertices[tri.x].position.x, riverVertices[tri.x].position.z);
        float2 b = float2(riverVertices[tri.y].position.x, riverVertices[tri.y].position.z);
        float2 c = float2(riverVertices[tri.z].position.x, riverVertices[tri.z].position.z);

        /// 地形側の頂点を見下ろしの2Dに
        float2 p = float2(tvPos.x, tvPos.z);

        /// Barycentric座標で三角形内外判定を取る
        float2 v0 = b - a;
        float2 v1 = c - a;
        float2 v2 = p - a;

        float d00 = dot(v0,v0);
        float d01 = dot(v0,v1);
        float d11 = dot(v1,v1);
        float d20 = dot(v2,v0);
        float d21 = dot(v2,v1);

        float denom = d00 * d11 - d01 * d01;
        float u = (d11 * d20 - d01 * d21) / denom;
        float v = (d00 * d21 - d01 * d20) / denom;

        /// 内外の判定をとる
        bool inside = (u >= 0.0) && (v >= 0.0) && (u + v <= 1.0);

        float factor = 0.0;

        if(inside) {
            factor = 1.0; // 三角形内なら完全に川の高さ
        } else {
            // 三角形外でもfalloffRadius以内なら徐々に押す
            float dist = min(
                length(p - closestPointOnTriangle(p, a, b, c)), 
                falloffRadius
            );
            factor = saturate(1.0 - dist / falloffRadius);
        }

        // 三角形の平均高さを計算
        float riverHeight = (riverVertices[tri.x].position.y +
                             riverVertices[tri.y].position.y +
                             riverVertices[tri.z].position.y) / 3.0f;

        // より強く影響する三角形を優先
        if(factor > maxFactor) {
            maxFactor = factor;
            chosenHeight = riverHeight;
        }
    }

    // 高さを補間（川の高さに合わせて押し下げる）
    tvPos.y = lerp(tvPos.y, chosenHeight - 1.0f, maxFactor);

    terrainVertices[tvIndex].position.xyz = tvPos;
}
