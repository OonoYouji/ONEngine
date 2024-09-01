#include "Particle.hlsli"

struct Material {
	float4 color;
	float4x4 uvTransform;
};



Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

StructuredBuffer<Material> gMaterial : register(t1);



PSOutput main(VSOutput input) {
	PSOutput output;
	
	float2 texcoord = mul(float3(input.texcoord, 1), (float3x3) gMaterial[input.instanceId].uvTransform).xy;

	output.color = gTexture.Sample(gSampler, texcoord);
	output.color *= gMaterial[input.instanceId].color; 
	output.color.a = gMaterial[input.instanceId].color.a; 
	
	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}