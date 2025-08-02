#include "Skybox.hlsli"

ConstantBuffer<TexIndex> texIndex : register(b0);

TextureCube<float4> textures[] : register(t0);
SamplerState samplerState : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;

	float4 texColor = textures[texIndex.id].Sample(samplerState, input.uv);

	output.color = texColor;
	output.worldPosition = input.worldPosition;
	output.normal = float4(input.normal, 1.0f);
	output.flags = float4(0, 0, 0, 1);
	
	return output;
}

