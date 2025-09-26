#include "Terrain.hlsli"

#include "../../ConstantBufferData/Material.hlsli"


Texture2D<float4> texGrass : register(t0);
Texture2D<float4> texDirt : register(t1);
Texture2D<float4> texRock : register(t2);
Texture2D<float4> texSnow : register(t3);
StructuredBuffer<Material> material : register(t4);

SamplerState textureSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;
	
	float3x3 matUVTransform = MatUVTransformToMatrix(material[0].uvTransform);

	float4 blend = input.splatBlend;
	float2 uv = mul(float3(input.uv, 1), matUVTransform).xy;

	float4 grass = texGrass.Sample(textureSampler, uv);
	float4 dirt = texDirt.Sample(textureSampler, uv);
	float4 rock = texRock.Sample(textureSampler, uv);
	float4 snow = texSnow.Sample(textureSampler, uv);
	
	output.color = float4(0, 0, 0, 1);
	output.color += mul(grass, blend.r);
	output.color += mul(dirt, blend.g);
	output.color += mul(rock, blend.b);
	output.color += mul(snow, blend.a);
	
	output.normal = float4(input.normal, 1);
	output.wPosition = input.wPosition;
	output.flags = float4(material[0].postEffectFlags, material[0].entityId, input.index, 1);
	
	return output;
}