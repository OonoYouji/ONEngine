#include "Particle.hlsli"


struct Transform {
	float4x4 matWorld;
};

struct ViewProjection {
	float4x4 matVp;
};


ConstantBuffer<ViewProjection> gViewProjection : register(b0);

/// constant buffer -> structured buffer
//ConstantBuffer<Transform> gTransform : register(b1);
StructuredBuffer<Transform> gTransformArray : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;
	
	float4x4 matWvp = mul(gTransformArray[instanceId].matWorld, gViewProjection.matVp);
	output.position = mul(input.position, matWvp);
	output.texcoord = input.texcoord;
	output.normal   = normalize(mul(float4(input.normal, 1), gTransformArray[instanceId].matWorld)).xyz;

	return output;
}