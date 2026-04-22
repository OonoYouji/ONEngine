#include "SkinMesh.hlsli"

#include "../../ConstantBufferData/Transform.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "../../ConstantBufferData/Material.hlsli"


struct SkinMeshInstanceData {
	float4x4 matWorld;
	Material material;
};

ConstantBuffer<ViewProjection> viewProjection : register(b0);
StructuredBuffer<SkinMeshInstanceData> instanceData : register(t0);
StructuredBuffer<Well> matrixPalette : register(t1);


VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;

	Skinned skinned;
	
	/// 座標を確実に float4(pos, 1.0) として扱う
	float4 pos = float4(input.position.xyz, 1.0f);

	/// 1. スキニング計算 (Local Space)
	skinned.position = mul(pos, matrixPalette[input.index.x].matSkeletonSpace) * input.weight.x;
	skinned.position += mul(pos, matrixPalette[input.index.y].matSkeletonSpace) * input.weight.y;
	skinned.position += mul(pos, matrixPalette[input.index.z].matSkeletonSpace) * input.weight.z;
	skinned.position += mul(pos, matrixPalette[input.index.w].matSkeletonSpace) * input.weight.w;
	skinned.position.w = 1.0f;

	/// 法線の変換
	skinned.normal = mul(input.normal, (float3x3) matrixPalette[input.index.x].matSkeletonSpaceInverseTranspose) * input.weight.x;
	skinned.normal += mul(input.normal, (float3x3) matrixPalette[input.index.y].matSkeletonSpaceInverseTranspose) * input.weight.y;
	skinned.normal += mul(input.normal, (float3x3) matrixPalette[input.index.z].matSkeletonSpaceInverseTranspose) * input.weight.z;
	skinned.normal += mul(input.normal, (float3x3) matrixPalette[input.index.w].matSkeletonSpaceInverseTranspose) * input.weight.w;
	skinned.normal = normalize(skinned.normal);


	/// 2. インスタンスデータの取得
	float4x4 matWorld = instanceData[instanceId].matWorld;

	/// 3. ワールド・ビュー・プロジェクション変換
	float4x4 matWVP = mul(matWorld, viewProjection.matVP);
	output.position = mul(skinned.position, matWVP);
	
	output.worldPosition = mul(skinned.position, matWorld);
	output.uv = input.uv;
	output.normal = normalize(mul(skinned.normal, (float3x3) matWorld));
	
	output.instanceId = instanceId;

	return output;
}