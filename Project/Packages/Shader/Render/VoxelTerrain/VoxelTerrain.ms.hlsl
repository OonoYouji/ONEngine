#include "VoxelTerrain.hlsli"

/// ---------------------------------------------------
/// Buffers
/// ---------------------------------------------------

StructuredBuffer<Chunk> chunks : register(t0);


/// ---------------------------------------------------
/// Main
/// ---------------------------------------------------

[shader("mesh")]
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    
    
    
}