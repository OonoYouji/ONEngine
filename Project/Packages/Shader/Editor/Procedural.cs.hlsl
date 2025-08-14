#include "Terrain.hlsli"

#include "../Math/Noise/Noise.hlsli"
#include "../ConstantBufferData/ViewProjection.hlsli"

struct InstanceData {
	float4x4 matWorld;
};

AppendStructuredBuffer<InstanceData> instanceData : register(u0);

Texture2D<float4> vertexTexture : register(t0);
Texture2D<float4> splatBlendTexture : register(t1);
SamplerState textureSampler : register(s0);

/// 二値化のための関数
float Binarize(float value, float threshold) {
	return step(threshold, value);
}

float Hash(float2 p) {
	/// 0～1の範囲の疑似乱数を返す
	return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

float GetGradientMagnitude(float2 uv, float texelSize) {
	float hL = vertexTexture.SampleLevel(textureSampler, uv - float2(texelSize, 0), 0).g; // 左
	float hR = vertexTexture.SampleLevel(textureSampler, uv + float2(texelSize, 0), 0).g; // 右
	float hD = vertexTexture.SampleLevel(textureSampler, uv - float2(0, texelSize), 0).g; // 下
	float hU = vertexTexture.SampleLevel(textureSampler, uv + float2(0, texelSize), 0).g; // 上

	/// 差分でdx, dy計算
	float dx = hR - hL;
	float dy = hU - hD;

	/// 勾配の大きさ（ベクトルの長さ）
	return sqrt(dx * dx + dy * dy);
}

static const float size = 2000;

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	float2 uv = float2(DTid.xy) / float2(size, size);

	float grad = GetGradientMagnitude(uv, 1.0f / size);
	if (grad > 0.005f) {
		return;
	}

	float4 blend = splatBlendTexture.Sample(textureSampler, uv);
	if (blend.r < 0.1f) {
		return;
	}
	
	uint index = DTid.x + DTid.y * size;
	float2 seed = float2(DTid.x, DTid.y * 1.3f);
	float noiseValue = PerlinNoise(seed);

	
	/// 拡縮
	float scale = noiseValue;

	/// 回転量
	float angle = Hash(seed + 1.0f) * 6.2831853f;
	float cosA = cos(angle);
	float sinA = sin(angle);
		
	/// 地形の高さ
	float4 height = vertexTexture.Sample(textureSampler, uv);
	float normalizeHeight = height.g;

	float randX = Hash(seed + 10.0);
	float randZ = Hash(seed + 30.0);
	float3 offset = float3(randX - 0.5, 0.0f, randZ - 0.5) * 0.5;
		
	float3 worldPosition = float3(uv.x * 1000.0f, DenormalizeHeight(normalizeHeight), uv.y * 1000.0f);
	worldPosition += offset;

	/// 回転行列
	float4x4 matRotateY = float4x4(
		cosA, 0, sinA, 0,
		0, 1, 0, 0,
		-sinA, 0, cosA, 0,
		0, 0, 0, 1
	);
	
	/// スケール行列
	float4x4 matScale = float4x4(
		scale, 0, 0, 0,
		0, scale, 0, 0,
		0, 0, scale, 0,
		0, 0, 0, 1
	);
	
	/// 平行移動行列
	float4x4 matTranslate = float4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		worldPosition.x, worldPosition.y, worldPosition.z, 1.0f
	);
		
	InstanceData output;
	output.matWorld = mul(mul(matScale, matRotateY), matTranslate);

	instanceData.Append(output);
	
}