struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput vs_main(uint vID : SV_VertexID) {
    VSOutput output;
    output.uv = float2((vID << 1) & 2, vID & 2);
    output.position = float4(output.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}

Texture2D gMainTexture : register(t0);
SamplerState gSampler : register(s0);

// ACES Filmic Tonemapping
float3 ACESFilm(float3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 ps_main(VSOutput input) : SV_TARGET {
    float4 color = gMainTexture.Sample(gSampler, input.uv);
    
    // 露出調整 (簡易版)
    float3 x = color.rgb * 1.0f; 
    
    // HDR -> SDR (ACES Filmic)
    float3 mapped = ACESFilm(x);
    
    // Gamma Correction
    float3 corrected = pow(mapped, 1.0f / 2.2f);
    
    return float4(corrected, color.a);
}
