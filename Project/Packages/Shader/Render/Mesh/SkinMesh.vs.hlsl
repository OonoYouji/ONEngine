#include "SkinMesh.hlsli"

#include "../../ConstantBufferData/Transform.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"


ConstantBuffer<ViewProjection> viewProjection : register(b0);
ConstantBuffer<Transform> transform : register(b1);
StructuredBuffer<Well> matrixPalette : register(t0);


Skinned Skinning(VSInput input) {
	Skinned skinned;
	
	/// 位置の変換
	skinned.position = mul(input.position, matrixPalette[input.index.x].matSkeletonSpace) * input.weight.x;
	skinned.position += mul(input.position, matrixPalette[input.index.y].matSkeletonSpace) * input.weight.y;
	skinned.position += mul(input.position, matrixPalette[input.index.z].matSkeletonSpace) * input.weight.z;
	skinned.position += mul(input.position, matrixPalette[input.index.w].matSkeletonSpace) * input.weight.w;
	skinned.position.w = 1.0f;
		
	/// 法線の変換
	skinned.normal = mul(input.normal, (float3x3) matrixPalette[input.index.x].matSkeletonSpaceInverseTranspose) * input.weight.x;
	skinned.normal += mul(input.normal, (float3x3) matrixPalette[input.index.y].matSkeletonSpaceInverseTranspose) * input.weight.y;
	skinned.normal += mul(input.normal, (float3x3) matrixPalette[input.index.z].matSkeletonSpaceInverseTranspose) * input.weight.z;
	skinned.normal += mul(input.normal, (float3x3) matrixPalette[input.index.w].matSkeletonSpaceInverseTranspose) * input.weight.w;
	skinned.normal = normalize(skinned.normal);

	return skinned;
}


VSOutput main(VSInput input) {
	VSOutput output;
	Skinned skinned = Skinning(input);

	float4x4 matWVP = mul(transform.matWorld, viewProjection.matVP);
	output.position = mul(skinned.position, matWVP);
	output.worldPosition = mul(skinned.position, transform.matWorld);
	output.uv = input.uv;
	output.normal = normalize(mul(skinned.normal, (float3x3) transform.matWorld));
	
	return output;
}