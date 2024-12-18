#include "Model.hlsli"
#include "../Camera/Camera.hlsli"
#include "../Transform/Transform.hlsli"

ConstantBuffer<ViewProjection> gViewProjection : register(b0);
ConstantBuffer<Transform> gTransform : register(b1);

VSOutput main(VSInput input) {
	VSOutput output;
	
	float4x4 matWvp = mul(gTransform.matWorld, gViewProjection.matVp);
	output.position = mul(input.position, matWvp);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) (gTransform.matWorldInverseTranspose)));
	output.worldPosition = mul(input.position, gTransform.matWorld).xyz;

	return output;
}