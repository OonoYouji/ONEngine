#include "SkinMesh.hlsli"

#include "../../ConstantBufferData/Transform.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"


ConstantBuffer<ViewProjection> gViewProjection : register(b0);
ConstantBuffer<Transform> gTransform : register(b1);
StructuredBuffer<Well> gMatrixPalette : register(t0);


Skinned Skinning(VSInput input) {
	Skinned skinned;
	
	/// 位置の変換
	skinned.position = mul(input.position, gMatrixPalette[input.index.x].matSkeletonSpace) * input.weight.x;
	skinned.position += mul(input.position, gMatrixPalette[input.index.y].matSkeletonSpace) * input.weight.y;
	skinned.position += mul(input.position, gMatrixPalette[input.index.z].matSkeletonSpace) * input.weight.z;
	skinned.position += mul(input.position, gMatrixPalette[input.index.w].matSkeletonSpace) * input.weight.w;
	skinned.position.w = 1.0f;
		
	/// 法線の変換
	skinned.normal = mul(input.normal, (float3x3) gMatrixPalette[input.index.x].matSkeletonSpaceInverseTranspose) * input.weight.x;
	skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index.y].matSkeletonSpaceInverseTranspose) * input.weight.y;
	skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index.z].matSkeletonSpaceInverseTranspose) * input.weight.z;
	skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index.w].matSkeletonSpaceInverseTranspose) * input.weight.w;
	skinned.normal = normalize(skinned.normal);

	return skinned;
}


VSOutput main(VSInput input) {
	VSOutput output;
	Skinned skinned = Skinning(input);

	float4x4 matWVP = mul(gTransform.matWorld, gViewProjection.matVP);
	output.position = mul(skinned.position, matWVP);
	output.worldPosition = mul(skinned.position, gTransform.matWorld);
	output.uv = input.uv;
	output.normal = normalize(mul(skinned.normal, (float3x3) gTransform.matWorld));
	
	return output;
}