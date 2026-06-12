#include "ParticleSystem.hlsli"

StructuredBuffer<float4> materials : register(t1);
StructuredBuffer<uint> textureIds : register(t2);
Texture2D<float4> textures[] : register(t3);

SamplerState pointSampler : register(s0);

float4 main(VSOutput input) : SV_TARGET {
    uint instanceIndex = input.instanceId;
    float4 matColor = materials[instanceIndex];
    uint texID = textureIds[instanceIndex];

    float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (texID != 0xFFFFFFFF) {
        float2 uv = input.uv;
        // Basic UV transform could be applied here if passed in material
        texColor = textures[texID].Sample(pointSampler, uv);
    }

    float4 finalColor = texColor * matColor * input.color;
    
    if (finalColor.a <= 0.01f) {
        discard;
    }

    return finalColor;
}
