#include "BlendRenderTexture.hlsli"

cbuffer ViewProjection : register(b0) {
	float4x4 matVp;
}

VSOutput main(VSInput input) {
	VSOutput output;
	output.position = mul(input.position, matVp);
	output.texcoord = input.texcoord;
	return output;
}