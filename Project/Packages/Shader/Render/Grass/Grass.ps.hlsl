#include "Grass.hlsli"

//struct PSInput {
//	float4 position : SV_POSITION;
//	float3 normal : NORMAL;
//	float2 uv : TEXCOORD0;
//};

struct PSOutput {
	float4 color : SV_TARGET0;
	float4 wPosition : SV_TARGET1;
	float4 normal : SV_TARGET2;
	float4 flags : SV_TARGET3;
};

[shader("pixel")]
PSOutput PSMain(VertexOut input) {
	PSOutput output;
	
	output.color = float4(0, 1, 0, 1); // 緑色で草を描画
	output.wPosition = input.position; // ワールド位置を出力
	output.normal = float4(normalize(input.normal), 1); // 法線を出力
	output.flags = float4(1, 0, 0, 1); // 草フラグを立てる

	// 緑色で草を描画
	return output;
}
