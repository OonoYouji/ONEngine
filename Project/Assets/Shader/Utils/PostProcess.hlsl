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
Texture2D gBloomTexture : register(t1);
Texture2D gNormalTexture : register(t2);
Texture2D<uint2> gIDTexture : register(t3);
SamplerState gSampler : register(s0);

cbuffer PostProcessParams : register(b1) {
    float gThreshold;
    float gIntensity;
    float gExposure;
    float gPadding;
    float4 gOutlineColor;
    float gOutlineThreshold;
    float gOutlineWidth;
    float2 gPadding2;
};

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
    float4 baseColor = gMainTexture.Sample(gSampler, input.uv);
    float4 bloomColor = gBloomTexture.Sample(gSampler, input.uv);
    
    // --- Outline Extraction ---
    float outlineMask = 0.0;
    
    // Screen-space coordinates for Load
    int3 pos = int3(input.position.xy, 0);
    uint currentID = gIDTexture.Load(pos).x;
    float3 currentNormal = gNormalTexture.Load(pos).xyz * 2.0 - 1.0;
    
    // Neighbors (simple cross pattern)
    int offset = (int)max(1.0, gOutlineWidth);
    int3 offsets[4] = {
        int3(offset, 0, 0), int3(-offset, 0, 0),
        int3(0, offset, 0), int3(0, -offset, 0)
    };
    
    for (int i = 0; i < 4; ++i) {
        int3 nPos = pos + offsets[i];
        
        // ID Edge
        uint nID = gIDTexture.Load(nPos).x;
        if (nID != currentID) {
            outlineMask = 1.0;
            break;
        }
        
        // Normal Edge
        float3 nNormal = gNormalTexture.Load(nPos).xyz * 2.0 - 1.0;
        float normalDiff = 1.0 - saturate(dot(currentNormal, nNormal));
        if (normalDiff > gOutlineThreshold) {
            outlineMask = 1.0;
            break;
        }
    }
    
    // ブルームを加算合成 (パラメータを使用)
    float3 combined = baseColor.rgb * gExposure + bloomColor.rgb * gIntensity;
    
    // アウトライン適用 (トーンマッピング前に行うか後に行うかは好みだが、一旦前に適用)
    combined = lerp(combined, gOutlineColor.rgb, outlineMask * gOutlineColor.a);
    
    // HDR -> SDR (ACES Filmic)
    float3 mapped = ACESFilm(combined);
    
    // Gamma Correction
    float3 corrected = pow(mapped, 1.0f / 2.2f);
    
    return float4(corrected, baseColor.a);
}
