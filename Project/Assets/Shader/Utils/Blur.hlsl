struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D gMainTexture : register(t0);
SamplerState gSampler : register(s0);

// パラメータを渡すための簡易的な定数
// 本来はConstantBufferで渡すべきだが、今回はレジスタ節約のため
// 特殊な方法（頂点シェーダーからのUVずらし）を使う
cbuffer BlurParams : register(b1) {
    float2 gDirection; // (1, 0) or (0, 1)
    float gTextureSize;
};

VSOutput vs_main(uint vID : SV_VertexID) {
    VSOutput output;
    output.uv = float2((vID << 1) & 2, vID & 2);
    output.position = float4(output.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    float2 texelSize = 1.0f / gTextureSize;
    float3 result = 0;
    
    // ガウス重み
    float weight[5] = {0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216};
    
    result += gMainTexture.Sample(gSampler, input.uv).rgb * weight[0];
    
    for(int i = 1; i < 5; ++i) {
        result += gMainTexture.Sample(gSampler, input.uv + gDirection * texelSize * i).rgb * weight[i];
        result += gMainTexture.Sample(gSampler, input.uv - gDirection * texelSize * i).rgb * weight[i];
    }
    
    return float4(result, 1.0f);
}
