#include "Mesh.hlsli"

//Texture2D<float4> texture        : register(t0);
Texture2D<float4> textures[]     : register(t0);
SamplerState      textureSampler : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;

	//float4 textureColor = texture.Sample(textureSampler, input.uv);
	float4 textureColor = textures[0].Sample(textureSampler, input.uv);

	output.color = textureColor;

	return output;
}