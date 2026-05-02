struct Vertex {
    float3 position;
    float2 uv;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

// 定数バッファ (b0)
struct SceneData {
    float4x4 viewProj;
};
ConstantBuffer<SceneData> gSceneData : register(b0);

// 頂点バッファ (t0)
StructuredBuffer<Vertex> gVertices : register(t0);

VSOutput vs_main(uint vID : SV_VertexID) {
    VSOutput output;
    
    // gVertices を使用
    Vertex v = gVertices[vID];
    
    // gSceneData を使用
    output.position = mul(float4(v.position, 1.0f), gSceneData.viewProj);
    output.uv = v.uv;
    
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    // ピクセルシェーダーでも何らかの計算に含める（最適化防止）
    return float4(input.uv, 1.0f, 1.0f);
}
