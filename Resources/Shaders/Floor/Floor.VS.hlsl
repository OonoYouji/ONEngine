#include "Floor.hlsli"

/// view projection
cbuffer cbuffer0 : register(b0) {
	float4x4 matVp;
}

/// transform matrix
cbuffer cbuffer1 : register(b1) {
	float4x4 matTransform;
}


VSOutput main(VSInput input) {
	VSOutput output;
	
	float4x4 matWVP = mul(matTransform, matVp);
	output.position = mul(input.position, matWVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3)matTransform));

	return output;
}


