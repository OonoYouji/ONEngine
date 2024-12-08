#include "Particle.hlsli"

struct Material {
	float4   color;
	float4x4 matUVTransform;
	int      isLighting;
};


//cbuffer material : register(b0) {
//	float4 materialColor;
//	float4x4 uvTransform;
//	int isLighting;
//}

cbuffer directionalLight : register(b1) {
	float4 lightColor;
	float3 lightDirection;
	float intensity;
}

Texture2D<float4>          gTexture   : register(t0);
SamplerState               gSampler   : register(s0);

StructuredBuffer<Material> gMaterials : register(t1);


PSOutput main(VSOutput input) {
	PSOutput output;

	uint instanceId = input.instanceId;

	float2 texcoord = mul(float3(input.texcoord, 1), (float3x3) gMaterials[instanceId].matUVTransform).xy;
	float4 texColor = gTexture.Sample(gSampler, texcoord);
	
	/// Half Lambert
	if (gMaterials[instanceId].isLighting) {
		float NdotL = dot(normalize(input.normal), -lightDirection);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color = gMaterials[instanceId].color * texColor * lightColor * cos * intensity;
		output.color.a = gMaterials[instanceId].color.a * texColor.a;
	} else {
		output.color = gMaterials[instanceId].color * texColor;
	}
	
	/// pixelの破棄
	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}