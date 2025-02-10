#include "Mesh.hlsli"

#include "../ConstantBufferData/Transform.hlsli"
#include "../ConstantBufferData/ViewProjection.hlsli"

ConstantBuffer<Transform>      transform      : register(b0);
ConstantBuffer<ViewProjection> viewProjection : register(b1);

VSOutput main(VSInput input) {
	VSOutput output;

	float4x4 matWVP = mul(transform.matWorld, viewProjection.matVP);

	output.position = mul(input.position, matWVP);
	output.normal   = mul(input.normal, (float3x3) transform.matWorld);
	output.uv       = input.uv;
	
	return output;
}