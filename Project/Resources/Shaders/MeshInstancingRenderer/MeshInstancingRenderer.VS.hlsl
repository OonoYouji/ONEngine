#include "MeshInstancingRenderer.hlsli"

struct Transform {
	float4x4 matWorld;
};

struct ViewProjection {
	float4x4 matVp;
};


ConstantBuffer<ViewProjection> gViewProjection : register(b0);
StructuredBuffer<Transform> gTransformArray : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;
	
	float4x4 matWvp = mul(gTransformArray[instanceId].matWorld, gViewProjection.matVp);
	output.position = mul(input.position, matWvp);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) (gTransformArray[instanceId].matWorld)));

	return output;
}