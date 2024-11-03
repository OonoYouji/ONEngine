#include "Blur.hlsli"

Texture2D gSourceTexture : register(t0);
SamplerState gSampler : register(s0);

// 定数バッファ
cbuffer BlurData : register(b0) {
	float2 texSize;
	int radius; // ブラー半径
	float sigma; // ガウシアンシグマ
};

float Gaussian(float x, float sigma) {
	return exp(-0.5 * (x * x) / (sigma * sigma)) / (sqrt(2.0 * 3.14159265) * sigma);
}

PSOutput main(VSOutput input) {
	PSOutput output;
	float4 color = float4(0, 0, 0, 0);
	float totalWeight = 0.0;
    
    // Horizontal blur
	for (int i = -radius; i <= radius; ++i) {
		float weight = Gaussian(float(i), sigma);
		color += gSourceTexture.Sample(gSampler, input.texcoord + float2(i * texSize.x, 0)) * weight;
		totalWeight += weight;
	}
	for (int j = -radius; j <= radius; ++j) {
		float weight = Gaussian(float(j), sigma);
		color += gSourceTexture.Sample(gSampler, input.texcoord + float2(0, j * texSize.y)) * weight;
		totalWeight += weight;
	}

    // Normalize the color
	output.color = color / totalWeight;

	return output;
}