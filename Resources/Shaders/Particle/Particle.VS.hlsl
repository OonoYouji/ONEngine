#include "Particle.hlsli"

struct Transform {
	float4x4 matWorld;
};

cbuffer ViewProjection : register(b0) {
	float4x4 matViewProjection;
}

StructuredBuffer<Transform> gTransforms : register(t0);


VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;

	float4x4 matWVP = mul(gTransforms[instanceId].matWorld, matViewProjection);

	output.position = mul(input.position, matWVP);
	output.normal = mul(input.normal, (float3x3) gTransforms[instanceId].matWorld);
	output.texcoord = input.texcoord;
	output.instanceId = instanceId;

	return output;
}