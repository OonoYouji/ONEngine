#include "Sprite.hlsli"

struct TextureID {
	uint id;
};

StructuredBuffer<TextureID> textureIDs     : register(t0);
Texture2D<float4>           textures[]     : register(t1);
SamplerState                textureSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;
	
	float4 textureColor = textures[textureIDs[input.instanceId].id].Sample(textureSampler, input.uv);
	output.color = textureColor;
	
	return output;
}
