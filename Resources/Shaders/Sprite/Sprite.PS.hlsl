#include "Sprite.hlsli"

struct Material {
	float4 color;
};


ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;

	output.color1 = gTexture.Sample(gSampler, input.texcoord);
	output.color1 *= gMaterial.color;

	output.color2 = output.color1;

	return output;
}