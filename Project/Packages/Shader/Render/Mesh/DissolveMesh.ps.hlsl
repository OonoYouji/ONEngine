#include "Mesh.hlsli"

#include "../../ConstantBufferData/Material.hlsli"

struct TextureId {
	uint id;
};

struct DissolveParams {
	uint id;
	uint dissolveCompare;
	float threshold;
	float edgeWidth;
	float4 edgeColor;
};


static const uint DISSOLVE_COMPARE_LESS = 0;
static const uint DISSOLVE_COMPARE_GREATER = 1;


StructuredBuffer<Material> materials : register(t0);
StructuredBuffer<DissolveParams> dissolveParams : register(t1);
StructuredBuffer<TextureId> textureIds : register(t2);

Texture2D<float4> textures[] : register(t3);
SamplerState textureSampler : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;

	DissolveParams dissolveParam = dissolveParams[input.instanceId];
	float4 dissolveTextureColor = textures[dissolveParam.id].Sample(textureSampler, input.uv);
	
	// Calculate luminance (Rec. 709 standard weights)
	float dissolveValue = dot(dissolveTextureColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

	float edge = 0.0f;
	if (dissolveParam.dissolveCompare == DISSOLVE_COMPARE_LESS) {
		if (dissolveValue > dissolveParam.threshold) {
			discard;
		}
		// Calculate edge glow
		edge = smoothstep(dissolveParam.threshold - dissolveParam.edgeWidth, dissolveParam.threshold, dissolveValue);
	} else if (dissolveParam.dissolveCompare == DISSOLVE_COMPARE_GREATER) {
		if (dissolveValue < dissolveParam.threshold) {
			discard;
		}
		// Calculate edge glow
		edge = smoothstep(dissolveParam.threshold + dissolveParam.edgeWidth, dissolveParam.threshold, dissolveValue);
	}
    
	Material material = materials[input.instanceId];
	float2 uv = mul(float3(input.uv, 1), MatUVTransformToMatrix(material.uvTransform)).xy;
	float4 textureColor = textures[textureIds[input.instanceId].id].Sample(textureSampler, uv);
	
	output.color = textureColor * material.baseColor;
	
	// Apply edge glow
	output.color.rgb += edge * dissolveParam.edgeColor.rgb * dissolveParam.edgeColor.a;

	output.worldPosition = input.worldPosition;
	output.normal = float4(input.normal, 1.0f);
	output.flags = float4(material.postEffectFlags, material.entityId, 0, 1);

	if (output.color.a == 0.0f) {
		discard;
	}
	
	return output;
}