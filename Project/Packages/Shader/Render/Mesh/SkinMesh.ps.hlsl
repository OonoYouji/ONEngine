#include "SkinMesh.hlsli"
#include "../../ConstantBufferData/Material.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"


struct SkinMeshInstanceData {
	float4x4 matWorld;
	Material material;
};

ConstantBuffer<ViewProjection> viewProjection : register(b0);

/// インスタンスインデックスを受け取る定数バッファ
cbuffer InstanceIndex : register(b1) {
	uint gInstanceIndex;
};

StructuredBuffer<SkinMeshInstanceData> instanceData : register(t0);

/// テクスチャ配列は t2
Texture2D<float4> textures[] : register(t2);
SamplerState textureSampler : register(s0);


PSOutput main(VSOutput input) {
	PSOutput output;
	
	/// インスタンスごとのマテリアル取得 (VSから渡されたIDを使用)
	Material material = instanceData[input.instanceId].material;
	
	/// テクスチャサンプリング
	float4 textureColor = textures[material.baseTextureId].Sample(textureSampler, input.uv);
	float4 materialColor = material.baseColor;
	
	/// カラー合成
	output.color = textureColor * materialColor;
	
	/// 表示確認用フォールバック
	if (output.color.r == 0 && output.color.g == 0 && output.color.b == 0) {
		output.color = materialColor;
	}
	
	output.worldPosition = input.worldPosition;
	output.normal = float4(input.normal.xyz, 1.0f);
	output.flags = float4(material.postEffectFlags, (float)material.entityId, 0, 1);

	if (output.color.a == 0.0f) {
		discard;
	}
	
	return output;
}