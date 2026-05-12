struct Vertex {
    float3 position;
    float4 color;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

#include "../Schema/Schema.hlsli"

ConstantBuffer<SceneData> gSceneData : register(b0);
StructuredBuffer<Vertex> gVertices : register(t0);

VSOutput vs_main(uint vID : SV_VertexID) {
    VSOutput output;
    Vertex v = gVertices[vID];
    output.position = mul(float4(v.position, 1.0f), gSceneData.viewProj);
    output.color = v.color;
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    return input.color;
}
