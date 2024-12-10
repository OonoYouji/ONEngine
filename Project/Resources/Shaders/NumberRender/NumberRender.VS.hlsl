#include "NumberRender.hlsli"
#include "../Camera/Camera.hlsli"

struct Transform {
	float4x4 matTransform;
};

/// buffer 
ConstantBuffer<ViewProjection> gViewProjection : register(b0);
StructuredBuffer<Transform>    gTransform      : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;

	float4x4 matWVP   = mul(gTransform[instanceId].matTransform, gViewProjection.matVp);
	output.position   = mul(input.position, matWVP);
	output.texcoord   = input.texcoord;
	output.instanceId = instanceId;

	return output;
}