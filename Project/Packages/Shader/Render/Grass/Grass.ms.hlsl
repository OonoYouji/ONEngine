#include "Grass.hlsli"

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1,1,1)]
void MSMain(uint3 groupThreadID : SV_GroupThreadID,
            inout TriangleStream<VertexOut> triStream)
{
    // 1本の草（三角形1つ）を描画
    for (int i = 0; i < 3; ++i)
    {
        VertexOut v;
        v.position = float4(bladeVertices[i], 1.0); // SV_POSITION は構造体内で
        v.normal   = float3(0,1,0);                 // 法線
        v.uv       = float2(bladeVertices[i].x + 0.5, bladeVertices[i].y);
        triStream.Append(v);
    }
}
