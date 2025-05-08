#include "MeshShaderTest.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"


StructuredBuffer<VertexInput> vertexInputs : register(t1);
StructuredBuffer<Index> gIndices : register(t2);

ConstantBuffer<ViewProjection> viewProjection : register(b1);

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
	uint groupIndex : SV_GroupIndex,
	uint3 DTid : SV_DispatchThreadID,
	in payload PayloadType payload,
	out vertices MSOutput vers[255],
	out indices uint3 tris[255]
	//out primitives PrimitiveOutput primitives[1]
) {

	if (groupIndex != 0) {
		return;
	}
	
	
	SetMeshOutputCounts(
		bufferLength.vertexInputLength,
		bufferLength.indexLength
	);
	
	float4x4 world = float4x4(
	   1, 0, 0, 0,
	   0, 1, 0, 0,
	   0, 0, 1, 0,
	   0, 0, 0, 1
	);
	
	float4x4 mat = mul(world, viewProjection.matVP);
	
	// 頂点設定
	for (int i = 0; i < bufferLength.vertexInputLength; ++i) {
		vers[i].position = mul(vertexInputs[i].position, mat);
		vers[i].normal   = vertexInputs[i].normal;
		vers[i].uv       = vertexInputs[i].uv;
	}

	for (i = 0; i < bufferLength.indexLength; ++i) {
		tris[i] = gIndices[i].index;
	}
	
}



//Meshlet meshlet = meshlets[payload.meshletIndex];
//SetMeshOutputCounts(
//	meshlet.vertexCount,
//	meshlet.indexCount
//);

//[unroll]
//for (uint i = 0; i < meshlet.vertexCount; ++i) {
//	VertexInput vi = vertexInputs[meshlet.vertexOffset + i];
//	vers[i].position = vi.position;
//	vers[i].normal = vi.normal;
//	vers[i].uv = vi.uv;
//}

////[unroll]
//for (i = 0; i < meshlet.vertexCount; ++i) {
//	uint idx0 = gIndices[meshlet.indexOffset + i].index.x - meshlet.vertexOffset;
//	uint idx1 = gIndices[meshlet.indexOffset + i].index.y - meshlet.vertexOffset;
//	uint idx2 = gIndices[meshlet.indexOffset + i].index.z - meshlet.vertexOffset;
//	tris[i] = uint3(idx0, idx1, idx2);
//}

