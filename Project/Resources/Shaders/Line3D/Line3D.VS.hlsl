#include "Line3D.hlsli"

struct ViewProjection {
	float4x4 matVp;
};

ConstantBuffer<ViewProjection> gViewProjection : register(b0);


VSOutput main(VSInput input) {
	VSOutput output;
	
	output.position = mul(input.position, gViewProjection.matVp);
	output.color = input.color;

	return output;
}