#include "Bloom.hlsli"

Texture2D gSourceTexure : register(t0); // 元の画像
SamplerState gSampler : register(s0);

cbuffer bloomData : register(b0) {
	float bloomIntensity = 0.7f; // ブルームの強度
	float bloomThreshold = 1.0f; // 輝度閾値
}



/// 
float4 ExtractBrightParts(float4 color) {
    // 輝度を計算し、閾値より高い部分だけを取り出す
	float luminance = dot(color.rgb, float3(0.2126, 0.7152, 0.0722)); // 輝度計算
	if (luminance > bloomThreshold) {
		return color;
	}
	return float4(0, 0, 0, 0); // 輝度が低い部分は無視
}


float4 BlurTexture(float2 texcoord) {
	float4 result = float4(0, 0, 0, 0);
	float offset = 1.0 / 512.0; // テクスチャのサンプル間隔

    // 周辺のピクセルから色を平均化（簡易的なボックスフィルタ）
	result += gSourceTexure.Sample(gSampler, texcoord + float2(-offset, -offset));
	result += gSourceTexure.Sample(gSampler, texcoord + float2(offset, -offset));
	result += gSourceTexure.Sample(gSampler, texcoord + float2(-offset, offset));
	result += gSourceTexure.Sample(gSampler, texcoord + float2(offset, offset));

	result /= 4.0; // 平均化

	return result;
}


PSOutput main(VSOutput input) {
	PSOutput output;

	// 元の色
	float4 originalColor = gSourceTexure.Sample(gSampler, input.texcoord);

	float4 brightParts = ExtractBrightParts(originalColor);
	float4 blurredBrightParts = BlurTexture(input.texcoord);

    // 元の画像とブルーム効果を合成
	float4 bloomColor = brightParts + blurredBrightParts * bloomIntensity;
    // 合成結果を返す
	output.color = originalColor + bloomColor;

	return output;
}