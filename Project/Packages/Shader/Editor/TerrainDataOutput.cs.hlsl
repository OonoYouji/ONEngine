#include "Terrain.hlsli"

ConstantBuffer<TerrainSize> terrainSize : register(b0);
RWStructuredBuffer<TerrainVertex> vertices : register(u0);
RWTexture2D<float4> outputVertexTexture : register(u1);
RWTexture2D<float4> outputSplatBlendTexture : register(u2);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint x = DTid.x;
	uint y = DTid.y;

	if (x >= terrainSize.terrainWidth || y >= terrainSize.terrainHeight) {
		return;
	}

	uint index = y * terrainSize.terrainWidth + x;
	TerrainVertex vertex = vertices[index];

	float4 pos = float4(0, 0, 0, 1);

	/// 高さを0~1に正規化
	pos.y = NormalizeHeight(vertex.position.y);
	
	outputVertexTexture[int2(x, y)] = pos;
	outputSplatBlendTexture[int2(x, y)] = vertex.splatBlend;
}