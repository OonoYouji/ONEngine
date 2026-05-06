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

struct PSOutput {
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    uint2 idFlags : SV_Target2;
};

PSOutput ps_main(VSOutput input) {
    PSOutput output;
    output.color = input.color;
    output.normal = float4(0.5f, 0.5f, 0.5f, 1.0f);
    output.idFlags = uint2(0, 0);
    return output;
}
