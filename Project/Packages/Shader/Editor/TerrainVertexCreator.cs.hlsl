#include "Terrain.hlsli"
#include "../Math/Noise/Noise.hlsli"

struct Index {
	uint value;
};

ConstantBuffer<TerrainSize> terrainSize : register(b0);
RWStructuredBuffer<TerrainVertex> vertices : register(u0);
RWStructuredBuffer<Index> indices : register(u1);

Texture2D<float4> vertexTexture : register(t0);
Texture2D<float4> splatBlendTexture : register(t1);
SamplerState textureSampler : register(s0);


float4 ModifySplatBlend(float4 _splatBlend, float2 _uv, float _noiseScale, float _noiseStrength) {
	float noiseR = PerlinNoise(_uv * _noiseScale + float2(10.0, 0.0));
	float noiseG = PerlinNoise(_uv * _noiseScale + float2(20.0, 0.0));
	float noiseB = PerlinNoise(_uv * _noiseScale + float2(30.0, 0.0));
	float noiseA = PerlinNoise(_uv * _noiseScale + float2(40.0, 0.0));

	_splatBlend.r = saturate(_splatBlend.r + (noiseR - 0.5) * _noiseStrength);
	_splatBlend.g = saturate(_splatBlend.g + (noiseG - 0.5) * _noiseStrength);
	_splatBlend.b = saturate(_splatBlend.b + (noiseB - 0.5) * _noiseStrength);
	_splatBlend.a = saturate(_splatBlend.a + (noiseA - 0.5) * _noiseStrength);

	float sum = _splatBlend.r + _splatBlend.g + _splatBlend.b + _splatBlend.a;
	if (sum > 0) {
		_splatBlend /= sum;
	}

	return _splatBlend;
}



[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint x = DTid.x;
	uint y = DTid.y;

	// -----------------------
	// 頂点生成
	// -----------------------
	if (x < terrainSize.terrainWidth && y < terrainSize.terrainHeight) {
		uint vertexIndex = y * terrainSize.terrainWidth + x;

		float2 uv = float2(
			x / (float) terrainSize.terrainWidth,
			y / (float) terrainSize.terrainHeight
		);

		float4 pos = float4((float) x, 0.0f, (float) y, 1.0f);
		float4 position = vertexTexture.Sample(textureSampler, uv);
		float4 blend = splatBlendTexture.Sample(textureSampler, uv);
		
		TerrainVertex v;
		v.position = pos;
		v.position.y = DenormalizeHeight(position.y);
		v.uv = float2(uv.x, -uv.y);
		v.normal = float3(0, 1, 0);
		v.splatBlend = blend;
		v.index = vertexIndex;

		vertices[vertexIndex] = v;
	}

	// -----------------------
	// インデックス生成（右下のセルのみ）
	// -----------------------
	if (x < terrainSize.terrainWidth - 1 && y < terrainSize.terrainHeight - 1) {
		// このスレッドが担当するセルの左上インデックス
		uint i0 = y * terrainSize.terrainWidth + x;
		uint i1 = y * terrainSize.terrainWidth + (x + 1);
		uint i2 = (y + 1) * terrainSize.terrainWidth + x;
		uint i3 = (y + 1) * terrainSize.terrainWidth + (x + 1);

		// 1セル＝6 index なので書き込み開始位置を計算
		uint indexStart = (y * (terrainSize.terrainWidth - 1) + x) * 6;
		
		// 三角形1（CCW: i0 → i2 → i1）
		indices[indexStart + 0].value = i0;
		indices[indexStart + 1].value = i2;
		indices[indexStart + 2].value = i1;

		// 三角形2（CCW: i2 → i3 → i1）
		indices[indexStart + 3].value = i2;
		indices[indexStart + 4].value = i3;
		indices[indexStart + 5].value = i1;
	}
}
