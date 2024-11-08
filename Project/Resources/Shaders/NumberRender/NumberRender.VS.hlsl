#include "NumberRender.hlsli"

struct Transform {
	float4x4 matTransform;
};

struct ViewProjection {
	float4x4 matVP;
};


/// buffer 
ConstantBuffer<ViewProjection> gViewProjection : register(b0);
ConstantBuffer<Transform>      gTransform      : register(b1);


VSOutput main(VSInput input) {
	VSOutput output;

	float4x4 matWVP = gTransform.matTransform * gViewProjection.matVP;
	output.position = mul(input.position, matWVP);
	output.texcoord = input.texcoord;

	return output;
}