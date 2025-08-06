#include "Terrain.hlsli"

#include "../../ConstantBufferData/Transform.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"


ConstantBuffer<ViewProjection> viewProjection : register(b0);
ConstantBuffer<Transform> transform : register(b1);

VSOutput main(VSInput input) {
	VSOutput output;

	float4x4 matWVP = mul(transform.matWorld, viewProjection.matVP);

	output.position   = mul(input.position, matWVP);
	output.wPosition  = mul(input.position, (float4x4) transform.matWorld);
	output.normal     = mul(input.normal, (float3x3) transform.matWorld);
	output.uv         = input.uv;
	output.splatBlend = input.splatBlend;
	output.index      = input.index;
	
	return output;
}