#include "Mesh.hlsli"

#include "../ConstantBufferData/Transform.hlsli"
#include "../ConstantBufferData/ViewProjection.hlsli"

struct InstanceOffset {
	uint offset;
};

ConstantBuffer<ViewProjection> viewProjection : register(b0);
StructuredBuffer<Transform>    transforms     : register(t0);

ConstantBuffer<InstanceOffset> instanceOffset : register(b1);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;

	uint instanceIndex = instanceId + instanceOffset.offset;
	float4x4 matWVP = mul(transforms[instanceIndex].matWorld, viewProjection.matVP);

	output.position = mul(input.position, matWVP);
	output.normal = mul(input.normal, (float3x3) transforms[instanceIndex].matWorld);
	output.uv       = input.uv;
	
	return output;
}