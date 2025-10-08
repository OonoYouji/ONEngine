// Grass.hlsli
struct VertexOut
{
    float4 position; // 頂点位置（Pixel Shader へ渡す）
    float3 normal;      // 法線
    float2 uv;   // UV
};

// 草の三角形1本分の頂点
static const float3 bladeVertices[3] = {
    float3(0, 0, 0),       // 根元
    float3(-0.05, 0.5, 0), // 左上
    float3(0.05, 0.5, 0)   // 右上
};
