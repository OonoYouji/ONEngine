#include "Terrain.hlsli"

Texture2D<float4> texGrass : register(t0);
Texture2D<float4> texDirt : register(t1);
Texture2D<float4> texRock : register(t2);
Texture2D<float4> texSnow : register(t3);

SamplerState textureSampler : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;
	
	float4 blend = input.splatBlend;
	
	float4 grass = texGrass.Sample(textureSampler, input.uv);
	float4 dirt = texDirt.Sample(textureSampler, input.uv);
	float4 rock = texRock.Sample(textureSampler, input.uv);
	float4 snow = texSnow.Sample(textureSampler, input.uv);
	
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