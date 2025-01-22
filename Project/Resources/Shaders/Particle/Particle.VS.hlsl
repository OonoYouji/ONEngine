#include "Particle.hlsli"
#include "../Camera/Camera.hlsli"
#include "../Transform/Transform.hlsli"

ConstantBuffer<ViewProjection> gViewProjection : register(b0);
StructuredBuffer<Transform> gTransformArray : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;
	
	float4x4 matWvp = mul(gTransformArray[instanceId].matWorld, gViewProjection.matVp);
	output.position = mul(input.position, matWvp);
	output.texcoord = input.texcoord;
	output.normal   = normalize(mul(float4(input.normal, 1), gTransformArray[instanceId].matWorld)).xyz;
	output.worldPosition = mul(input.position, gTransformArray[instanceId].matWorld).xyz;

	return output;
}