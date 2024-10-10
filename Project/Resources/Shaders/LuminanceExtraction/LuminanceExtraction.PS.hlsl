#include "LuminanceExtraction.hlsli"

Texture2D gSourceTexture : register(t0);
SamplerState gSampler : register(s0);

// 定数バッファ
cbuffer LuminanceExtractData : register(b0) {
	float bloomThreshold; // 輝度閾値
};

// 輝度を計算するための関数
float4 ExtractBrightParts(float4 color) {
    // 輝度を計算する（YUV輝度計算）
	float luminance = dot(color.rgb, float3(0.2126, 0.7152, 0.0722)); // YUV輝度計算
	if (luminance > bloomThreshold) {
		return color; // 輝度が閾値より高い場合は色をそのまま返す
	}
	return float4(0, 0, 0, 0); // 輝度が閾値以下の場合は透明にする
}

PSOutput main(VSOutput input) {
	PSOutput output;
	// テクスチャから色をサンプリング
	float4 originalColor = gSourceTexture.Sample(gSampler, input.texcoord);
	output.color = ExtractBrightParts(originalColor);
	// 輝度抽出処理
	return output;
}

