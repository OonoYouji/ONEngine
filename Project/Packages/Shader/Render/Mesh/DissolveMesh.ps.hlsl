#include "Mesh.hlsli"

#include "../../ConstantBufferData/Material.hlsli"

struct TextureId {
	uint id;
};

struct DissolveParams {
    uint id;
    float threshold;
};


StructuredBuffer<Material>       materials      : register(t0);
StructuredBuffer<DissolveParams> dissolveParams : register(t1);
StructuredBuffer<TextureId>      textureIds     : register(t2);

Texture2D<float4> textures[] : register(t3);
SamplerState textureSampler : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;

    DissolveParams dissolveParam = dissolveParams[input.instanceId];
    float4 dissolveTextureColor = textures[dissolveParam.id].Sample(textureSampler, input.uv);
    if (dissolveTextureColor.r < dissolveParam.threshold) {
        discard;
    }
    

	Material material = materials[input.instanceId];
	float2 uv = mul(float3(input.uv, 1), MatUVTransformToMatrix(material.uvTransform)).xy;
	float4 textureColor = textures[textureIds[input.instanceId].id].Sample(textureSampler, uv);
	
	output.color = textureColor * material.baseColor;
	output.worldPosition = input.worldPosition;
	output.normal = float4(input.normal, 1.0f);
	output.flags = float4(material.postEffectFlags, material.entityId, 0, 1);

	if (output.color.a == 0.0f) {
		discard;
	}
	
	return output;
}