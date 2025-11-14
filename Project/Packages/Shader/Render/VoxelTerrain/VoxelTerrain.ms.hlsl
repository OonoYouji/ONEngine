#include "VoxelTerrain.hlsli"

/// ---------------------------------------------------
/// Buffers
/// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);


/// ---------------------------------------------------
/// Main
/// ---------------------------------------------------

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(8, 8, 1)]
void main(
    uint3 DTid : SV_DispatchThreadID,
    in payload Payload asPayload,
    out vertices VertexOut verts[256],
    out indices uint3 indices[256]) {

        
	SetMeshOutputCounts(256, 256);

	for (uint i = 0; i < 256; i++) {
		verts[i].position = float4(0, 0, 0, 1);
		verts[i].worldPosition = float4(0, 0, 0, 1);
		verts[i].normal = float3(0, 1, 0);
        
		indices[i] = uint3(0, 0, 0);
	}

	for (uint i = 0; i < 256; i++) {
		indices[i] = uint3(0, 1, 2);
	}
    
    
}