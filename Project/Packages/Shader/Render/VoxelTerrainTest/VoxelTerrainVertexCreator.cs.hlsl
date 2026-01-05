
struct Vertex {
	float4 position;
	float3 normal;
	//float2 uv;
	//uint refTextureId;
};

struct Triangle {
	uint3 indices;
};


struct VoxelTerrainData {
	
};

AppendStructuredBuffer<Vertex> vertexBuffer : register(u0);
AppendStructuredBuffer<Triangle> indexBuffer : register(u1);

Texture3D<float4> volumeTextures[] : register(t0);
SamplerState textureSampler : register(s0);


static const int3 kCubeOffset[8] = {
	int3(0, 0, 0), int3(1, 0, 0), int3(1, 1, 0), int3(0, 1, 0),
	int3(0, 0, 1), int3(1, 0, 1), int3(1, 1, 1), int3(0, 1, 1)
};


[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    
}