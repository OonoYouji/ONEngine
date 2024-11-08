#include "NumberRender.hlsli"

struct Material {
	float4 color;
	float  space;
};

/// buffer
ConstantBuffer<Material> gMaterial : register(b0);

/// texture
Texture2D<float4>        gTexture  : register(t0);
SamplerState             gSampler  : register(s0);

float4 main(VSOutput input) : SV_TARGET0 {
	float4 output;
	
	float4 texColor = gTexture.Sample(gSampler, input.texcoord);
	output          = gMaterial.color * texColor;
	
	return output;
}