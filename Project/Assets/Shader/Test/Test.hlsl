struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

// 定数バッファ
struct SceneData {
    float4x4 viewProj;
};
ConstantBuffer<SceneData> gSceneData : register(b0);

// テクスチャ
Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

// Manual Vertex Fetching のテスト: Input Layout 不要
VSOutput vs_main(uint vID : SV_VertexID) {
    VSOutput output;
    
    // テスト用にダミーの三角形を出力
    float2 positions[3] = { float2(0.0, 0.5), float2(0.5, -0.5), float2(-0.5, -0.5) };
    float2 uvs[3] = { float2(0.5, 0.0), float2(1.0, 1.0), float2(0.0, 1.0) };
    
    output.position = mul(float4(positions[vID % 3], 0.0f, 1.0f), gSceneData.viewProj);
    output.uv = uvs[vID % 3];
    
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    // 本来はテクスチャからサンプルするが、テストなので色を返す
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
