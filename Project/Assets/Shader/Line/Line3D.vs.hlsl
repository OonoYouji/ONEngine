#include "Line3D.hlsli"
#include "../ConstantBufferData/Transform.hlsli"
#include "../ConstantBufferData/ViewProjection.hlsli"

ConstantBuffer<Transform>      tranfrom       : register(b0);
ConstantBuffer<ViewProjection> viewProjection : register(b1);


VSOutput main(VSInput input) {
	VSOutput output;

	float4x4 matWVP = mul(tranfrom.matWorld, viewProjection.matVP);

	output.position = mul(input.position, matWVP);
	output.color    = input.color;
	
	return output;
}