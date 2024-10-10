#include "Floor.hlsli"

/// texcoord transform matrix
cbuffer material : register(b0) {
	float4x4 uvTransform;
};

Texture2D<float4> gTex : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;
	
	/// texcoord transform
	float2 texcoord = mul(float3(input.texcoord, 1), (float3x3)uvTransform).xy;
	output.color = gTex.Sample(gSampler, texcoord);

	if (output.color.a == 0.0f) {
		discard;
	}
	
	return output;
}