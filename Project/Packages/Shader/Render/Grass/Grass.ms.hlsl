#include "Grass.hlsli"

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void MSMain(uint3 groupThreadID : SV_GroupThreadID,
			out vertices VertexOut verts[3],
			out indices uint3 indis[1]) {
	
	SetMeshOutputCounts(3, 1);

	uint vertIndex = groupThreadID.x * 3; // 1本の草につき3頂点
	uint indisIndex = groupThreadID.x; // 1本の草につき1つの三角形
	
	for(int i = 0; i < 3; ++i) {
		verts[vertIndex + i].position = float4(bladeVertices[i], 1.0); // 根元
		verts[vertIndex + i].normal = float3(0, 1, 0); // 法線
		verts[vertIndex + i].uv = float2(bladeVertices[i].x + 0.5, bladeVertices[i].y);
	}
	
	indis[indisIndex] = uint3(vertIndex, vertIndex + 1, vertIndex + 2);

 //   // 1本の草（三角形1つ）を描画
	//for (int i = 0; i < 3; ++i) {
	//	VertexOut v;
	//	v.position = float4(bladeVertices[i], 1.0); // SV_POSITION は構造体内で
	//	v.normal = float3(0, 1, 0); // 法線
	//	v.uv = float2(bladeVertices[i].x + 0.5, bladeVertices[i].y);
	//	triStream.Append(v);
	//}
}
