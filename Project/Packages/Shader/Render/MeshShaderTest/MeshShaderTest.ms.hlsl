#include "MeshShaderTest.hlsli"


[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID) {
    
}

//void MSMain(in PayloadType payload : SV_Payload,
//            out vertices VertexOut verts[3],
//            out indices uint3 indices[1]) {
//	verts[0].position = float3(-1, -1, 0);
//	verts[1].position = float3(1, -1, 0);
//	verts[2].position = float3(0, 1, 0);
    
//	verts[0].normal = verts[1].normal = verts[2].normal = float3(0, 0, 1);
//	verts[0].uv = float2(0, 0);
//	verts[1].uv = float2(1, 0);
//	verts[2].uv = float2(0.5, 1);

//	indices[0] = uint3(0, 1, 2);
//}
