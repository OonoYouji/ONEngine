#include "Grass.hlsli"

#include "../../ConstantBufferData/Material.hlsli"


struct PSOutput {
	float4 color : SV_TARGET0;
	float4 wPosition : SV_TARGET1;
	float4 normal : SV_TARGET2;
	float4 flags : SV_TARGET3;
};

/// 草の共通マテリアル
ConstantBuffer<ConstantBufferMaterial> material : register(b2);
Texture2D<float4> textures[] : register(t2);
SamplerState texSampler : register(s0);

[shader("pixel")]
PSOutput PSMain(VertexOut input) {
	PSOutput output;
	
	float4 texColor = textures[material.intValues.z].Sample(texSampler, input.uv);
	
	output.color = texColor * material.baseColor;
	output.wPosition = input.position; // ワールド位置を出力
	output.normal = float4(normalize(input.normal), 1); // 法線を出力
	output.flags = float4(material.intValues.x, material.intValues.y, 0, 1); // 草フラグを立てる

	// 緑色で草を描画
	return output;
}
