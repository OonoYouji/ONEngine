#include "VoxelTerrain.hlsli"
#include "../../ConstantBufferData/Material.hlsli"

struct PSOutput {
	float4 color : SV_Target0;
	float4 worldPos : SV_Target1;
	float4 normal : SV_Target2;
	float4 flags : SV_Target3;
};

ConstantBuffer<ConstantBufferMaterial> material : register(b4);

PSOutput main(VertexOut input) {
	PSOutput output;

	// 1. 法線の正規化
	float3 N = normalize(input.normal);

	// ---------------------------------------------------------
	// 地形の色分け処理
	// ---------------------------------------------------------
	
	// 草の色 (緑)
	float4 grassColor = float4(0.1f, 0.55f, 0.2f, 1.0f);
	// 岩/岸壁の色 (茶色～灰色)
	float4 rockColor = float4(0.35f, 0.3f, 0.25f, 1.0f);

	// ブレンド率の計算
	// N.y (法線の上方向成分) を使用します。
	// 1.0 = 真上, 0.0 = 真横, -1.0 = 真下
	float slopeBlend = smoothstep(0.4f, 0.8f, N.y);

	// 岩と草の色を合成
	float4 terrainColor = lerp(rockColor, grassColor, slopeBlend);

	// ---------------------------------------------------------

	// 元の色情報(頂点カラー * マテリアル色) に 地形色 を乗算
	output.color = material.baseColor * terrainColor;
	
	output.worldPos = input.worldPosition;
	output.normal = float4(N, 1);
	output.flags = float4(material.intValues.x, material.intValues.y, 0, 1);

	if (output.color.a <= 0.001f) {
		discard;
	}
	
	return output;
}