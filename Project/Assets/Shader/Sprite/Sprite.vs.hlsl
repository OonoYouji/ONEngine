#include "Sprite.hlsli"

#include "../ConstantBufferData/ViewProjection.hlsli"
#include "../ConstantBufferData/Transform.hlsli"

ConstantBuffer<ViewProjection> viewProjection : register(b0);
StructuredBuffer<Transform>    transforms     : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;

	float4x4 matWVP = mul(transforms[instanceId].matWorld, viewProjection.matVP);
	
	output.position = mul(input.position, matWVP);
	output.uv       = input.uv;

	return output;
}