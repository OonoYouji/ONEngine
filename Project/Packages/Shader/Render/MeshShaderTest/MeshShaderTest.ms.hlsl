#include "MeshShaderTest.hlsli"


[shader("mesh")]
[outputtopology("triangle")]
[numthreads(32, 1, 1)]
void main(
	uint3 dispatchThreadID : SV_DispatchThreadID,
	in payload PayloadType payload,
	out vertices MSOutput vers[3],
	out indices uint3 tri[1]) {

	SetMeshOutputCounts(3, 1);
	
	vers[0].position = float4(0, 0, 0, 1);
	vers[0].normal = float3(0, 0, 1);
	vers[0].uv = float2(0, 0);
	//vers[0].color = float4(1, 0, 0, 1);

	vers[1].position = float4(1280, 0, 0, 1);
	vers[1].normal = float3(0, 0, 1);
	vers[1].uv = float2(0, 0);
	//vers[1].color = float4(0, 1, 0, 1);
	
	vers[2].position = float4(0, 720, 0, 1);
	vers[2].normal = float3(0, 0, 1);
	vers[2].uv = float2(0, 0);
	//vers[2].color = float4(0, 0, 1, 1);
	
	tri[0] = uint3(0, 1, 2);
}

