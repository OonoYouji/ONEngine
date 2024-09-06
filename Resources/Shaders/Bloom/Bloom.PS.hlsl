#include "Bloom.hlsli"

Texture2D gSourceTexure : register(t0); // 元の画像
Texture2D gLuminanceTexture : register(t1); // 抽出した輝度の画像
SamplerState gSampler : register(s0);

cbuffer bloomData : register(b0) {
	float bloomIntensity; // ブルームの強度
	float2 texSize;
}

PSOutput main(VSOutput input) {
	PSOutput output;
	float4 originalColor = gSourceTexure.Sample(gSampler, input.texcoord);
	float4 bloomColor = gLuminanceTexture.Sample(gSampler, input.texcoord) * bloomIntensity;
	output.color =  originalColor + bloomColor;
	return output;
}
