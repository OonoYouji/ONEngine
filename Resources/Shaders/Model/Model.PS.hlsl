#include "Model.hlsli"


struct Material {
	float4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);


float4 main(VSOutput input) : SV_TARGET0 {
	float4 outputColor;

	outputColor = gTexture.Sample(gSampler, input.texcoord);
	outputColor *= gMaterial.color;

	return outputColor;
}