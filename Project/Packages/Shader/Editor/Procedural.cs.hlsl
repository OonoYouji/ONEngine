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

float Hash(float2 p) {
	return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

float GetGradientMagnitude(float2 uv, float texelSize) {
	float hL = vertexTexture.SampleLevel(textureSampler, uv - float2(texelSize, 0), 0).g;
	float hR = vertexTexture.SampleLevel(textureSampler, uv + float2(texelSize, 0), 0).g;
	float hD = vertexTexture.SampleLevel(textureSampler, uv - float2(0, texelSize), 0).g;
	float hU = vertexTexture.SampleLevel(textureSampler, uv + float2(0, texelSize), 0).g;

	float dx = hR - hL;
	float dy = hU - hD;
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

	float2 seed = uv * 16.0f; // 周波数を上げると密度の塊が細かくなる
	float noiseValue = PerlinNoise(seed); // [-1,1]
	noiseValue = noiseValue * 0.5f + 0.5f; // [0,1]

	float density = saturate(noiseValue * 0.001f);

	float randVal = Hash(seed + float2(123.456, 789.101));
	if (randVal > density) {
		return;
	}

	float scale = lerp(2.0f, 4.0f, noiseValue);
	float angle = Hash(seed + 1.0f) * 6.2831853f;
	float cosA = cos(angle);
	float sinA = sin(angle);

	float4 height = vertexTexture.Sample(textureSampler, uv);
	float normalizeHeight = height.g;

	float randX = Hash(seed + 10.0);
	float randZ = Hash(seed + 30.0);
	float3 offset = float3(randX - 0.5, 0.0f, randZ - 0.5) * 0.5;

	float3 worldPosition = float3(
        uv.x * 1000.0f,
        DenormalizeHeight(normalizeHeight),
        uv.y * 1000.0f
    );
	worldPosition += offset;


	float4x4 matRotateY = float4x4(
        cosA, 0, sinA, 0,
        0, 1, 0, 0,
        -sinA, 0, cosA, 0,
        0, 0, 0, 1
    );

	float4x4 matScale = float4x4(
        scale, 0, 0, 0,
        0, scale, 0, 0,
        0, 0, scale, 0,
        0, 0, 0, 1
    );

	float4x4 matTranslate = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        worldPosition.x, worldPosition.y, worldPosition.z, 1
    );

	InstanceData output;
	output.matWorld = mul(mul(matScale, matRotateY), matTranslate);
	instanceData.Append(output);
}
