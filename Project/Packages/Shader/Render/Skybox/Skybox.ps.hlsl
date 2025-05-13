#include "Skybox.hlsli"

#include "../../ConstantBufferData/Material.hlsli"

ConstantBuffer<TexIndex> texIndex : register(b0);

TextureCube<float4> textures[] : register(t0);
SamplerState samplerState : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;

	float4 texColor = textures[texIndex.id].Sample(samplerState, input.uv);

	output.color = float4(1, 1, 1, 1);
	output.worldPosition = input.worldPosition;
	output.normal = float4(input.normal, 1.0f);
	output.flags = float4(1, 1, 1, 1);
	
	return output;
}

