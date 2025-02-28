#include "Mesh.hlsli"

#include "../ConstantBufferData/Material.hlsli"

struct TextureId {
	uint id;
};

//Texture2D<float4> texture        : register(t0);
StructuredBuffer<Material>  materials      : register(t0);
StructuredBuffer<TextureId> textureIds     : register(t1);
Texture2D<float4>           textures[]     : register(t2);
SamplerState                textureSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;

	float4 textureColor  = textures[textureIds[input.instanceId].id].Sample(textureSampler, input.uv);
	float4 materialColor = materials[input.instanceId].color;
	
	output.color = textureColor * materialColor;

	if (output.color.a == 0.0f) { ///< alpha == 0.0f ? pixel discard
		discard;
	}
	
	return output;
}