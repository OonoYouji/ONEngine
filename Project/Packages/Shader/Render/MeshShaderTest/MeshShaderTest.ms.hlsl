#include "MeshShaderTest.hlsli"

ConstantBuffer<BufferLength> bufferLength : register(b0);
StructuredBuffer<VertexInput> vertexInputs : register(t0);
StructuredBuffer<Index> gIndices : register(t1);

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
	uint groupIndex : SV_GroupIndex,
	uint3 DTid : SV_DispatchThreadID,
	//in payload PayloadType payload,
	out vertices MSOutput vers[6],
	out indices uint3 tris[2]
	//out primitives PrimitiveOutput primitives[1]
) {

	if (groupIndex != 0) {
		return;
	}
	
	SetMeshOutputCounts(
		bufferLength.vertexInputLength, 
		bufferLength.indexLength
	);
	  
	// インデックスから頂点を取得
	uint3 indis = gIndices[groupIndex].index;
	float4x4 world = float4x4(
	   1, 0, 0, 0,
	   0, 1, 0, 0,
	   0, 0, 1, 0,
	   0, 0, 0, 1
	);
	
	// 頂点設定
	for (int i = 0; i < bufferLength.vertexInputLength; ++i) {
		vers[i].position = mul(vertexInputs[i].position, world);
		vers[i].normal = vertexInputs[i].normal;
		vers[i].uv = vertexInputs[i].uv;
	}


	for (i = 0; i < bufferLength.indexLength; ++i) {
		tris[i] = gIndices[i].index;
	}
}

