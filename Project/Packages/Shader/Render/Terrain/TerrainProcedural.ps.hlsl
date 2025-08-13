#include "TerrainProcedural.hlsli"

#include "../../ConstantBufferData/Material.hlsli"

struct TextureId {
	uint value;
};

ConstantBuffer<TextureId> textureId : register(b0);

Texture2D<float4> textures[] : register(t0);
SamplerState textureSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;
	
	float4 textureColor = textures[textureId.value].Sample(textureSampler, input.uv);
	if (textureColor.a < 0.1f) {
		discard;
	}
	
	output.color = textureColor;
	output.normal = float4(input.normal, 1);
	output.wPosition = input.wPosition;
	output.flags = float4(0, 0, 0, 1);

	return output;
}