#include "Sprite.hlsli"

cbuffer Material : register(b0) {
	float4 materialColor;
	float4x4 uvTransform;
	int isLighting;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;

	float2 texcoord = mul(float3(input.texcoord, 1), (float3x3) uvTransform).xy;
	output.color = gTexture.Sample(gSampler, texcoord);
	output.color *= materialColor;

	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}