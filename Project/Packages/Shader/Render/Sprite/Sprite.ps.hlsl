#include "Sprite.hlsli"

#include "../../ConstantBufferData/Material.hlsli"

StructuredBuffer<Material> materials : register(t0);
Texture2D<float4> textures[] : register(t1);
SamplerState textureSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;
	Material material = materials[input.instanceId];
	
	/// TextureSampling
	float3x3 matUV = MatUVTransformToMatrix(material.uvTransform);
	float2 uv = mul(float3(input.uv, 1), matUV).xy;
	float4 baseTexColor = textures[material.baseTextureId].Sample(textureSampler, uv);

	float4 outputColor = baseTexColor * material.color;

	if (outputColor.a < 0.01) {
		discard;
	}
	
	output.color = outputColor;
	output.worldPosition = input.position;
	output.normal = float4(0, 0, 1, 1);
	output.flags = float4(material.postEffectFlags, material.entityId, 0, 1);
	
	return output;
}
