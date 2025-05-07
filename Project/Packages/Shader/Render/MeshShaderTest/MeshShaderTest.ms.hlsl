#include "MeshShaderTest.hlsli"

//StructuredBuffer<VertexInput> vertices : register(t0);
//StructuredBuffer<Index> indices : register(t1);

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(32, 1, 1)]
void main(
	//uint groupIndex : SV_GroupIndex,
	out vertices MSOutput vers[3],
	out indices uint3 tris[1]) {

	SetMeshOutputCounts(3, 1);

	//if (groupIndex < 1) {
	//	tris[groupIndex] = indices[groupIndex].index;
	//}
	
	//if (groupIndex < 3) {
	//	vers[groupIndex].position = vertices[groupIndex].position;
	//	vers[groupIndex].normal = vertices[groupIndex].normal;
	//	vers[groupIndex].uv = vertices[groupIndex].uv;
	//}
	
	
	vers[0].position = float4(0, 0, 0, 1);
	vers[0].normal = float3(0, 0, 1);
	vers[0].uv = float2(0, 0);

	vers[1].position = float4(1280, 0, 0, 1);
	vers[1].normal = float3(0, 0, 1);
	vers[1].uv = float2(0, 0);
	
	vers[2].position = float4(0, 720, 0, 1);
	vers[2].normal = float3(0, 0, 1);
	vers[2].uv = float2(0, 0);
	
	tris[0] = uint3(0, 1, 2);
}

