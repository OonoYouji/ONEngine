#include "Terrain.hlsli"

struct Index {
	uint value;
};



ConstantBuffer<TerrainSize> terrainSize : register(b0);
RWStructuredBuffer<TerrainVertex> vertices : register(u0);
RWStructuredBuffer<Index> indices : register(u1);

//Texture2D<float4> vertexTexture : register(t0);
//Texture2D<float4> splatBlendTexture : register(t1);
//SamplerState textureSampler : register(s0);

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

		float4 pos = float4(
			(float) x - terrainSize.terrainWidth * 0.5f,
			0.0f,
			(float) y - terrainSize.terrainHeight * 0.5f,
			1.0f
		);

		//float4 position = vertexTexture.SampleLevel(textureSampler, uv);
		
		TerrainVertex v;
		v.position = pos;
		v.uv = float2(uv.x, -uv.y);
		v.normal = float3(0, 1, 0);
		v.splatBlend = float4(1.0f - uv.x, 1.0f - uv.y, 0.0f, 0.0f);
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

		// 三角形1
		indices[indexStart + 0].value = i0;
		indices[indexStart + 1].value = i1;
		indices[indexStart + 2].value = i2;

		// 三角形2
		indices[indexStart + 3].value = i2;
		indices[indexStart + 4].value = i1;
		indices[indexStart + 5].value = i3;
	}
}
