#include "Blur.hlsli"

Texture2D gSourceTexture : register(t0);
SamplerState gSampler : register(s0);

// 定数バッファ
cbuffer LuminanceExtractData : register(b0) {
	float2 texSize;
};

float Gaussian(float x, float sigma) {
	return exp(-0.5 * (x * x) / (sigma * sigma)) / (sqrt(2.0 * 3.14159265) * sigma);
}

PSOutput main(VSOutput input) {
	PSOutput output;
	output.color = float4(0, 0, 0, 0);

	float totalWeight = 0.0;
	int radius = 16; // Blur radius
	float sigma = 4.0f;
    
    // Horizontal blur
	for (int i = -radius; i <= radius; ++i) {
		float weight = Gaussian(float(i), sigma);
		output.color += gSourceTexture.Sample(gSampler, input.texcoord + float2(i * texSize.x, 0)) * weight;
		totalWeight += weight;
	}

	for (int j = -radius; j <= radius; ++j) {
		float weight = Gaussian(float(j), sigma);
		output.color += gSourceTexture.Sample(gSampler, input.texcoord + float2(0.0f, j * texSize.y)) * weight;
		totalWeight += weight;
	}

    // Normalize the color
	output.color /= totalWeight;

	return output;
}