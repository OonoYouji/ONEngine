#include "Terrain.hlsli"
#include "../Math/Noise/Noise.hlsli"
#include "../ConstantBufferData/ViewProjection.hlsli"

struct Data {
	float size;
};

struct InstanceData {
	float4x4 matWorld;
	float4 minBounds;
	float4 maxBounds;
};

ConstantBuffer<Data> data : register(b0);

AppendStructuredBuffer<InstanceData> instanceData : register(u0);

Texture2D<float4> vertexTexture      : register(t0);
Texture2D<float4> splatBlendTexture  : register(t1);
Texture2D<float4> arrangementTexture : register(t2); /// 配置用の輝度テクスチャ
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

float GetLuminance(float4 _color) {
    return dot(_color.rgb, float3(0.2126, 0.7152, 0.0722));
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	float size = data.size;
	float2 uv = float2(DTid.xy) / float2(size, size);

	/// 配置用テクスチャの輝度を用いて、配置判定を行う
	float2 arrangementUV = float2(uv.x, 1.0f - uv.y);
	float4 arrangement = arrangementTexture.Sample(textureSampler, arrangementUV);
	float luminance = GetLuminance(arrangement);
	if (luminance < 0.2f) {
		return;
	}


	/// 勾配が急な箇所には配置しない
	float grad = GetGradientMagnitude(uv, 1.0f / size);
	if (grad > 0.005f) {
		return;
	}

	/// スプラットブレンドの草がないところには配置しない
	float4 blend = splatBlendTexture.Sample(textureSampler, uv);
	if (blend.r < 0.1f) {
		return;
	}

	float2 seed = uv * 16.0f;
	float noiseValue = PerlinNoise(seed); // [-1,1]
	noiseValue = noiseValue * 0.5f + 0.5f; // [0,1]

	float density = saturate(noiseValue * 0.001f);

	float randVal = Hash(seed + float2(123.456, 789.101));
	if (randVal > density) {
		return;
	}

	float scale = lerp(2.0f, 8.0f, noiseValue);
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
	output.minBounds = float4(-0.5f * scale, 0.0f, -0.5f * scale, 1.0f);
	output.maxBounds = float4(0.5f * scale, scale, 0.5f * scale, 1.0f);
	instanceData.Append(output);
}
