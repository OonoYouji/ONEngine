#include "NumberRender.hlsli"

struct Material {
	float4 color;
	float2 textureSize;
	float2 tileSize;
	float  space;
};

struct NumberDigit {
	uint number;
};

/// buffer
ConstantBuffer<Material>      gMaterial    : register(b0);
StructuredBuffer<NumberDigit> gNumberDigit : register(t1);

/// texture
Texture2D<float4>             gTexture     : register(t0);
SamplerState                  gSampler     : register(s0);

float4 main(VSOutput input) : SV_TARGET0 {
	float4 output;
	
	/// 表示する桁
	uint     number         = gNumberDigit[input.instanceId].number;

	float2   tileScale = gMaterial.tileSize / gMaterial.textureSize;
	float3x3 matUVTransform = float3x3(
		tileScale.x, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		number * tileScale.x, 0.0f, 1.0f
	);

	float3 texcoord = mul(float3(input.texcoord, 1), matUVTransform);
	float4 texColor = gTexture.Sample(gSampler, texcoord.xy);
	output          = gMaterial.color * texColor;

	if (output.a == 0.0f) {
		discard;
	}
	
	return output;
}