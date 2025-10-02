#include "River.hlsli"

#include "../../ConstantBufferData/Material.hlsli"

ConstantBuffer<ConstantBufferMaterial> material : register(b1);
Texture2D<float4> textures[] : register(t0);
SamplerState textureSampler : register(s0);

PSOutput main(VSOutput input) { 
    PSOutput output;

    float2 uv = mul(float3(input.uv, 1), MatUVTransformToMatrix(material.uvTransform)).xy;
    float4 texColor = textures[material.intValues.z].Sample(textureSampler, uv);
    output.color = material.baseColor * texColor;
    output.wPosition = input.wPosition;
    output.normal = float4(input.normal, 1);
    output.flags = float4(material.intValues.x, material.intValues.y, 0, 1);

    return output;
}