#include "SkinMesh.hlsli"
#include "../../ConstantBufferData/Material.hlsli"

struct TextureId {
	uint id;
};

ConstantBuffer<Material> material : register(b0);
ConstantBuffer<TextureId> textureId : register(b1);
Texture2D<float4> textures[] : register(t0);
SamplerState textureSampler : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;
	
	float4 textureColor = textures[textureId.id].Sample(textureSampler, input.uv);
	float4 materialColor = material.color;
	
	output.color = textureColor * materialColor;
	output.worldPosition = input.worldPosition;
	output.normal = float4(input.normal, 1.0f);
	output.flags = float4(1, 0, 1, 1);

	if (output.color.a == 0.0f) {
		discard;
	}
	
	return output;
}