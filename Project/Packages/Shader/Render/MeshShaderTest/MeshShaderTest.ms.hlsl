#include "MeshShaderTest.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"


StructuredBuffer<VertexInput> vertexInputs : register(t1);
StructuredBuffer<Index> gIndices : register(t2);
StructuredBuffer<Meshlet> meshlets : register(t3);

ConstantBuffer<ViewProjection> viewProjection : register(b1);



[shader("mesh")]
[outputtopology("triangle")]
[numthreads(64, 1, 1)]
void main(
	uint groupIndex : SV_GroupIndex,
	uint3 DTid : SV_DispatchThreadID,
	//in payload PayloadType payload,
	out vertices VertexOutput vers[256],
	out indices uint3 tris[256]) {

	if (groupIndex != 0) {
		return;
	}

	uint meshletIndex = DTid.x;
	Meshlet meshlet = meshlets[meshletIndex];
	SetMeshOutputCounts(meshlet.vertexCount, meshlet.triangleCount);


	float4x4 matWVP = viewProjection.matVP;

	uint i = 0;
	
	/// インデックスの読み込み
	//for (i = 0; i < 1; ++i) {
	//	uint base = meshlet.triangleOffset + i;
	//	uint3 index = uint3(
	//		gIndices[base].index.x - meshlet.vertexOffset,
	//		gIndices[base].index.y - meshlet.vertexOffset,
	//		gIndices[base].index.z - meshlet.vertexOffset
	//	);
	//	tris[i] = index;
	//}
	
	/// どの頂点を使うかのインデックスを計算
	uint indexIndex[256];
	for (i = 0; i < meshlet.triangleCount; ++i) {
		uint base = meshlet.triangleOffset + i;
		indexIndex[i * 3 + 0] = gIndices[base].index.x;
		indexIndex[i * 3 + 1] = gIndices[base].index.y;
		indexIndex[i * 3 + 2] = gIndices[base].index.z;
	}

	/// 頂点の読み込み
	for (i = 0; i < meshlet.triangleCount; ++i) {
		for (int j = 0; j < 3; ++j) {
			uint index = i * 3 + j;
			VertexInput vi = vertexInputs[indexIndex[index]];
			VertexInput viResult = {
				mul(vi.position, matWVP),
				vi.normal,
				vi.uv
			};

			vers[index].position = viResult.position;
			vers[index].normal = viResult.normal;
			vers[index].uv = viResult.uv;
			vers[index].index = meshletIndex;
		}
	}


	/// インデックスの計算
	for (i = 0; i < meshlet.triangleCount; ++i) {
		uint3 index = uint3(
			i * 3 + 0,
			i * 3 + 1,
			i * 3 + 2
		);
		tris[i] = index;
	}
	

	//SetMeshOutputCounts(
	//	bufferLength.vertexInputLength,
	//	bufferLength.indexLength
	//);
	
	//float4x4 world = float4x4(
	//   1, 0, 0, 0,
	//   0, 1, 0, 0,
	//   0, 0, 1, 0,
	//   0, 0, 0, 1
	//);
	
	//float4x4 mat = mul(world, viewProjection.matVP);
	
	//// 頂点設定
	//for (int i = 0; i < bufferLength.vertexInputLength; ++i) {
	//	vers[i].position = mul(vertexInputs[i].position, mat);
	//	vers[i].normal   = vertexInputs[i].normal;
	//	vers[i].uv       = vertexInputs[i].uv;
	//}

	//for (i = 0; i < bufferLength.indexLength; ++i) {
	//	tris[i] = gIndices[i].index;
	//}
	
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

