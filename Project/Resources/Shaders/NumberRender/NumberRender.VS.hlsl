#include "NumberRender.hlsli"

struct Transform {
	float4x4 matTransform;
};

struct ViewProjection {
	float4x4 matVP;
};


/// buffer 
ConstantBuffer<ViewProjection> gViewProjection : register(b0);
StructuredBuffer<Transform>    gTransform      : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;

	float4x4 matWVP = gTransform[instanceId].matTransform * gViewProjection.matVP;
	output.position = mul(input.position, matWVP);
	output.texcoord = input.texcoord;

	return output;
}