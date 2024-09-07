#include "Bloom.hlsli"

Texture2D gSourceTexure : register(t0); // 元の画像
Texture2D gBlurredTexture : register(t1); // 抽出した輝度の画像
SamplerState gSampler : register(s0);

cbuffer bloomData : register(b0) {
	float bloomIntensity; // ブルームの強度
	float2 texSize;
}

PSOutput main(VSOutput input) {
	PSOutput output;
	//float4 originalColor = gSourceTexure.Sample(gSampler, input.texcoord);
	//float4 bloomColor = gBlurredTexture.Sample(gSampler, input.texcoord) * bloomIntensity;
	//output.color = (originalColor + bloomColor);

	// 元のテクスチャから色をサンプリング
	float4 originalColor = gSourceTexure.Sample(gSampler, input.texcoord);
	// ガウシアンブラー後のテクスチャから色をサンプリング
	float4 bloomColor = gBlurredTexture.Sample(gSampler, input.texcoord);
	// オリジナルの色とブルームカラーを加算してブルームエフェクトを作成
	float4 finalColor = originalColor + bloomColor * bloomIntensity;
	// 必要であれば、色をトーンマップまたは制限（例えばクリッピング）する
	output.color = saturate(finalColor);
	return output;
}
