#include "Line3D.hlsli"
#include "../ConstantBufferData/ViewProjection.hlsli"

ConstantBuffer<ViewProjection> viewProjection : register(b0);


VSOutput main(VSInput input) {
	VSOutput output;

	output.position = mul(input.position, viewProjection.matVP);
	output.color    = input.color;
	
	return output;
}