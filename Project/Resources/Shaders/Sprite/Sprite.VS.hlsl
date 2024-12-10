#include "Sprite.hlsli"
#include "../Camera/Camera.hlsli"

struct Transform {
	float4x4 matWorld;
};

ConstantBuffer<ViewProjection> gViewProjection : register(b0);
ConstantBuffer<Transform> gTransform : register(b1);

VSOutput main(VSInput input) {
	VSOutput output;

	float4x4 matWvp = mul(gTransform.matWorld, gViewProjection.matVp);
	output.position = mul(input.position, matWvp);
	output.texcoord = input.texcoord;

	return output;
}