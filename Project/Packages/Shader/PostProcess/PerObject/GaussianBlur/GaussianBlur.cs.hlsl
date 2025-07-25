// GaussianBlur.compute.hlsl

struct BlurParams {
	float2 texelSize; // (1.0f / width, 1.0f / height)
	int blurRadius; // サンプル数（例: 3）
	float sigma; // ガウス分布のσ
	int horizontal; // 1: 横方向, 0: 縦方向`
};

Texture2D<float4> sceneTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);
SamplerState textureSampler : register(s0);

static const BlurParams params = {
	float2(1.0f / 1920.0f, 1.0f / 1080.0f), // テクセルサイズ
	12, // ブラー半径
	1.0f, // σ
	1 // 横方向ブラー
};


float Gaussian(int x, float sigma) {
	return exp(-((x * x) / (2.0f * sigma * sigma)));
}

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	float2 uv = (float2) dispatchThreadId.xy * params.texelSize;

	float4 sum = float4(0, 0, 0, 0);
	float totalWeight = 0.0;

	for (int i = -params.blurRadius; i <= params.blurRadius; ++i) {
		int2 offset = params.horizontal != 0 ? int2(i, 0) : int2(0, i);
		float2 offsetUV = uv + params.texelSize * offset;

		float weight = Gaussian(i, params.sigma);
		sum += sceneTexture.SampleLevel(textureSampler, offsetUV, 0.0) * weight;
		totalWeight += weight;
	}

	outputTexture[dispatchThreadId.xy] = sum / totalWeight;
}
