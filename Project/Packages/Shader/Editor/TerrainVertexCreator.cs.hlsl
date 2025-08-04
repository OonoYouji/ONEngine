#include "Terrain.hlsli"

RWStructuredBuffer<TerrainVertex> vertices : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

}