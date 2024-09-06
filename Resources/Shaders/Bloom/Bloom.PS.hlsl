#include "Bloom.hlsli"

Texture2D gSourceTexure : register(t0); // 元の画像
SamplerState gSampler : register(s0);

cbuffer bloomData : register(b0) {
	float bloomIntensity; // ブルームの強度
	float bloomThreshold; // 輝度閾値
	float2 texelSize;
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

    // ガウスカーネルの重み
	float kernel[5] = {
		1.0 / 256.0, 4.0 / 256.0, 6.0 / 256.0, 4.0 / 256.0, 1.0 / 256.0
	};

    // オフセットのサイズ（テクスチャのサイズに応じて調整）
	float2 offset = texelSize;

    // 5x5のガウシアンカーネルでサンプリング（ブラー処理）
	for (int x = -2; x <= 2; ++x) {
		for (int y = -2; y <= 2; ++y) {
			result += gSourceTexure.Sample(gSampler, texcoord + float2(x, y) * offset) * kernel[abs(x) + abs(y) * 5];
		}
	}

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