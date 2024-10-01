#include "Particle.hlsli"


struct Transform {
	float4x4 matWorld;
};

struct ViewProjection {
	float4x4 matVp;
};


ConstantBuffer<ViewProjection> gViewProjection : register(b0);

/// constant buffer -> structured buffer
ConstantBuffer<Transform> gTransform : register(b1);

StructuredBuffer<Transform> gTransformArray : register(s1);

VSOutput main(VSInput input) {
	VSOutput output;
	
	float4x4 matWvp = mul(gTransform.matWorld, gViewProjection.matVp);
	output.position = mul(input.position, matWvp);
	output.texcoord = input.texcoord;
	output.normal   = normalize(mul(float4(input.normal, 1), gTransform.matWorld)).xyz;

	return output;
}