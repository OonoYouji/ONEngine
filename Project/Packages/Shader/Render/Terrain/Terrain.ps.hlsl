#include "Terrain.hlsli"

Texture2D<float4> texGrass : register(t0);
Texture2D<float4> texDirt : register(t1);
Texture2D<float4> texRock : register(t2);
Texture2D<float4> texSnow : register(t3);

SamplerState textureSampler : register(s0);

static const float3x3 uvTransform = float3x3(
	10, 00, 0,
	00, 10, 0,
	00, 00, 1
);


PSOutput main(VSOutput input) {
	PSOutput output;
	
	float4 blend = input.splatBlend;
	float2 uv = mul(float3(input.uv, 1), uvTransform).xy;
	
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
	output.flags = float4(1, 0, 0, 0);
	
	return output;
}