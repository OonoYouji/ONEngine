#include "../VoxelTerrain/VoxelTerrainCommon.hlsli"
#include "Table.hlsli"

struct Vertex {
	float4 position;
	float3 normal;
	//float2 uv;
	//uint refTextureId;
};

struct MarchingCube {
	float isoValue;
	float voxelSize;
};

struct ChunkIndex {
	uint value;
};


ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<MarchingCube> marchingCube : register(b1);
ConstantBuffer<ChunkIndex> chunkIndex : register(b2);

StructuredBuffer<Chunk> chunks : register(t0);

AppendStructuredBuffer<Vertex> OutVertiecs : register(u1);

Texture3D<float4> volumeTextures[] : register(t2);
// SamplerState textureSampler : register(s0);


static const uint2 EdgeIndex[12] = {
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 0 },
	{ 4, 5 },
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 4 },
	{ 0, 4 },
	{ 1, 5 },
	{ 2, 6 },
	{ 3, 7 }
};

static const float3 VertexOffset[8] = {
	{ 0, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 0, 1 },
	{ 1, 0, 1 },
	{ 1, 1, 1 },
	{ 0, 1, 1 }
};


float3 Interpolate(float3 p0, float3 p1, float v0, float v1) {
	float t = (marchingCube.isoValue - v0) / (v1 - v0);
	return lerp(p0, p1, t);
}

float3 CalcNormal(int3 p, uint textureId) {
	float dx = volumeTextures[textureId].Load(int4(p + int3(1, 0, 0), 0)).w
             - volumeTextures[textureId].Load(int4(p - int3(1, 0, 0), 0)).w;
	float dy = volumeTextures[textureId].Load(int4(p + int3(0, 1, 0), 0)).w
             - volumeTextures[textureId].Load(int4(p - int3(0, 1, 0), 0)).w;
	float dz = volumeTextures[textureId].Load(int4(p + int3(0, 0, 1), 0)).w
             - volumeTextures[textureId].Load(int4(p - int3(0, 0, 1), 0)).w;

	return normalize(float3(dx, dy, dz));
}




[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	if (any(DTid >= voxelTerrainInfo.textureSize - 1)) {
		return;
	}

	float density[8];
	float3 pos[8];
	
	uint textureId = chunks[chunkIndex.value].textureId;
	
	for (uint i = 0; i < 8; ++i) {
		int3 samplePos = int3(DTid) + VertexOffset[i];
		density[i] = volumeTextures[textureId].Load(int4(samplePos, 0)).w;
		pos[i] = (float3(samplePos) + 0.5) * marchingCube.voxelSize;
	}
	
	// 
	uint cubeIndex = 0;
	for (uint i = 0; i < 8; ++i) {
		cubeIndex |= (density[i] < marchingCube.isoValue) ? (1u << i) : 0;
	}
	
	uint edgeMask = EdgeTable[cubeIndex];
	if (edgeMask == 0) {
		return;
	}
	
	float3 edgeVertices[12];
	
	/// 
	for (uint i = 0; i < 12; ++i) {
		if ((edgeMask & (1u << i)) != 0) {
			uint2 edge = EdgeIndex[i];
			edgeVertices[i] = Interpolate(
				pos[edge.x],
				pos[edge.y],
				density[edge.x],
				density[edge.y]
			);
		}
	}
	
	/// 三角形の生成
	for (uint i = 0; i < 16; i += 3) {

		int edges[3];
		for (uint j = 0; j < 3; ++j) {
			edges[j] = TriTable[cubeIndex][i + j];
		}
		
		if (edges[0] == -1) {
			break;
		}

		
		Vertex v[3];
		for (uint j = 0; j < 3; ++j) {
			float3 pos = edgeVertices[edges[j]];
			float3 normal = CalcNormal(int3(DTid) + int3(VertexOffset[0]), textureId);

			v[j].position = float4(pos, 1.0f);
			v[j].normal = normal;

			OutVertiecs.Append(v[j]);
		}
	}

}