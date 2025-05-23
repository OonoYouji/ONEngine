#include "Effect.hlsli"

#include "../../ConstantBufferData/Material.hlsli"

struct TextureId {
	uint id;
};

StructuredBuffer<Material> materials : register(t0);
StructuredBuffer<TextureId> textureIds : register(t1);
Texture2D<float4> textures[] : register(t2);
SamplerState textureSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;

	float4 textureColor = textures[textureIds[input.instanceId].id].Sample(textureSampler, input.uv);
	float4 materialColor = materials[input.instanceId].color;
	
	output.color = textureColor * materialColor;
	output.worldPosition = input.worldPosition;
	output.normal = float4(input.normal, 1.0f);
	output.flags = float4(0, 0, 1, 1);

	if (output.color.a == 0.0f) {
		discard;
	}
	
	return output;
}