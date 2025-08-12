#include "../Math/Noise/Noise.hlsli"
#include "Terrain.hlsli"

struct InstanceData {
	float4x4 matWorld;
};

AppendStructuredBuffer<InstanceData> instanceData : register(u0);

Texture2D<float4> vertexTexture : register(t0);
SamplerState textureSampler : register(s0);

/// 二値化のための関数
float Binarize(float value, float threshold) {
	return step(threshold, value);
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	float2 uv = DTid.xy / float2(1000, 1000);
	
	float noiseValue = Binarize(PerlinNoise(uv * 10.0), 0.5f);
	
	/// 一定値を超えた場所にモデルを配置する
	if (noiseValue > 0.5f) {

		float4 height = vertexTexture.Sample(textureSampler, uv);
		float normalizeHeight = height.g;
		
		float3 worldPosition = float3(uv.x * 1000.0, DenormalizeHeight(normalizeHeight), uv.y * 1000.0);

		InstanceData output;
		output.matWorld = float4x4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			worldPosition.x, worldPosition.y, worldPosition.z, 1.0f
		);

		instanceData.Append(output);
	}
	
}