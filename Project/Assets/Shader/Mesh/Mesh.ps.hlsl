#include "Mesh.hlsli"

#include "../ConstantBufferData/Material.hlsli"

//Texture2D<float4> texture        : register(t0);
StructuredBuffer<Material> materials      : register(t0);
Texture2D<float4>          textures[]     : register(t1);
SamplerState               textureSampler : register(s0);

PSOutput main(VSOutput input) {
	PSOutput output;

	//float4 textureColor = texture.Sample(textureSampler, input.uv);
	float4 textureColor = textures[0].Sample(textureSampler, input.uv);
	float4 materialColor = materials[input.instanceId].color;
	
	output.color = textureColor * materialColor;

	if (output.color.a == 0.0f) { ///< alpha == 0.0f ? pixel discard
		discard;
	}
	
	return output;
}