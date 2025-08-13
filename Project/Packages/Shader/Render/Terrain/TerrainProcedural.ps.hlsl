#include "TerrainProcedural.hlsli"

#include "../../ConstantBufferData/Material.hlsli"

Texture2D<float4> textures[] : register(t0);
SamplerState textureSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;
	
	float4 textureColor = textures[1].Sample(textureSampler, input.uv);
	
	output.color = float4(0.427f, 0.573f, 0.027f, 1.0f) * textureColor;
	output.normal = float4(input.normal, 1);
	output.wPosition = input.wPosition;
	output.flags = float4(0, 0, 0, 1);

	return output;
}