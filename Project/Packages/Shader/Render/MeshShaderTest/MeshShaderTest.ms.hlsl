#include "MeshShaderTest.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"


StructuredBuffer<VertexInput> vertexInputs : register(t1);
StructuredBuffer<Index> gIndices : register(t2);
StructuredBuffer<Meshlet> meshlets : register(t3);
ByteAddressBuffer UniqueVertexIndices : register(t4);

ConstantBuffer<ViewProjection> viewProjection : register(b1);

static const float4x4 matWorld = float4x4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
);


uint3 UnpackPrimitive(uint primitive) {
    // Unpacks a 10 bits per index triangle from a 32-bit uint.
	return uint3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}

uint3 GetPrimitive(Meshlet m, uint index) {
	return UnpackPrimitive(gIndices[m.triangleOffset + index].index);
}

uint GetVertexIndex(Meshlet m, uint localIndex) {
	localIndex = m.vertexOffset + localIndex;

	/// 32bit固定なのでこの処理だけでよい
	return UniqueVertexIndices.Load(localIndex * 4);
}

VertexOutput GetVertexAttributes(uint meshletIndex, uint vertexIndex) {
	VertexInput v = vertexInputs[vertexIndex];

	VertexOutput vout;
	vout.position = mul(v.position, viewProjection.matVP);
	vout.uv = v.uv;
	//vout.PositionHS = mul(float4(v.Position, 1), Globals.WorldViewProj);
	vout.normal = mul(float4(v.normal, 0), matWorld).xyz;
	vout.index = meshletIndex;

	return vout;
}


[shader("mesh")]
[outputtopology("triangle")]
[numthreads(128, 1, 1)]
void main(
	uint GTid : SV_GroupThreadID,
	//uint groupIndex : SV_GroupIndex,
	uint groupId : SV_GroupID,
	//uint3 DTid : SV_DispatchThreadID,
	//in payload PayloadType payload,
	out vertices VertexOutput vers[64],
	out indices uint3 tris[126]) {

	Meshlet m = meshlets[groupId];
	SetMeshOutputCounts(m.vertexCount, m.triangleCount);

	//for (int i = 0; i < m.triangleCount; ++i) {
	//	uint3 triIndex = GetPrimitive(m, i);
	//	tris[i] = triIndex;
	//}

	//for (i = 0; i < m.vertexCount; ++i) {
	//	uint vertexIndex = GetVertexIndex(m, i);
	//	VertexOutput v = GetVertexAttributes(groupId, vertexIndex);
	//	vers[i] = v;
	//}


	if (GTid < m.triangleCount) {
		uint3 i = GetPrimitive(m, GTid);
		tris[GTid] = i;
	}

	if (GTid < m.vertexCount) {
		uint vertexIndex = GetVertexIndex(m, GTid);
		VertexOutput v = GetVertexAttributes(groupId, vertexIndex);
		vers[GTid] = v;
	}

}

