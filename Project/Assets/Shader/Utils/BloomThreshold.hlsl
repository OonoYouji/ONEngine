struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer PostProcessParams : register(b1) {
    float gThreshold;
    float gIntensity;
    float gExposure;
    float gPadding;
};

Texture2D gMainTexture : register(t0);
SamplerState gSampler : register(s0);

VSOutput vs_main(uint vID : SV_VertexID) {
    VSOutput output;
    output.uv = float2((vID << 1) & 2, vID & 2);
    output.position = float4(output.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    float4 color = gMainTexture.Sample(gSampler, input.uv);
    
    // 輝度の計算 (Rec.709)
    float luminance = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    // 閾値処理 (パラメータを使用)
    float3 brightColor = color.rgb * max(luminance - gThreshold, 0.0f) / max(luminance, 0.0001f);
    
    return float4(brightColor, color.a);
}
