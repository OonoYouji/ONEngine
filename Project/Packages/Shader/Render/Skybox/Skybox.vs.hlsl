#include "Skybox.hlsli"

#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "../../ConstantBufferData/Transform.hlsli"

ConstantBuffer<ViewProjection> viewProjection : register(b0);
ConstantBuffer<Transform>      transform      : register(b1);


VSOutput main(VSInput input) {
	VSOutput output;

	float4x4 matWVP = mul(transform.matWorld, viewProjection.matVP);
	
	output.position = mul(input.position, matWVP).xyww;
	output.worldPosition = mul(input.position, transform.matWorld);
	output.normal = float3(0, 0, 0);
	output.uv = input.position.xyz;
	
	return output;
}