#include "Skybox.hlsli"

#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "../../ConstantBufferData/Transform.hlsli"

ConstantBuffer<ViewProjection> viewProjection : register(b0);
ConstantBuffer<Transform>      transform      : register(b1);


VSOutput main(VSInput input) {
	VSOutput output;

	float4x4 matWVP = mul(transform.matWorld, viewProjection.matVP);
	
	output.position = mul(input.position, matWVP);
	output.worldPosition = mul(input.position, transform.matWorld);
	//output.normal = input.normal;
	output.uv = input.uv;
	return output;
}