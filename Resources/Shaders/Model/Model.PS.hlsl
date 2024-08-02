#include "Model.hlsli"


struct Material {
	float4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;

	output.color = gTexture.Sample(gSampler, input.texcoord);
	output.color *= gMaterial.color;
	
	if (output.color.a == 0.0f) {
		discard;
	}

	return output;

}