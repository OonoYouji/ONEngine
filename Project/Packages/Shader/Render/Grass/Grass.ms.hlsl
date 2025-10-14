//#include "Grass.hlsli"
#include "BladeInstance.hlsli"

#include "../../ConstantBufferData/ViewProjection.hlsli"

struct Time {
	float value;
};

ConstantBuffer<ViewProjection> viewProjection : register(b0);
StructuredBuffer<Time> time : register(t1);

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(kMaxGrassDataSize, 1, 1)]
void MSMain(uint3 DTid : SV_DispatchThreadID,
			uint3 Gid : SV_GroupThreadID,
			uint gIndex : SV_GroupIndex,
			in payload Payload asPayload,
			out vertices VertexOut verts[5],
			out indices uint3 indis[2]) {
	SetMeshOutputCounts(5, 2);
	
	uint index = asPayload.grassData[gIndex].index;
	BladeInstance instance = bladeInstances[index];
	
	float3 t = normalize(instance.tangent);
	float3 up = float3(0, 1, 0);
	float3 b = normalize(cross(t, up)); // 横方向のベクトル
	float3 b2 = normalize(cross(b, up)); // 横方向のベクトル
	//float3 b = float3(1, 0, 0); // 横方向のベクトル
	//float3 b2 = float3(0, 0, 1); // 横方向のベクトル
	float width = instance.scale;
	float height = 2.0f * instance.scale;
	
	float sinValue = sin(time[0].value + instance.random01 * 6.28);
	
	//float3(instance.position + float3(0, height, 0) + (normalize(instance.tangent) * sinValue)),
	
	float3 bladePoss[kMaxBladeVertexNum] = {
		/// 根本頂点
		float3(instance.position - float3(0, height, 0)),
		/// 他、上部の頂点
		float3(instance.position + b * -width + float3(0, height, 0)),
		float3(instance.position + b * width + float3(0, height, 0)),
		float3(instance.position + b2 * -width + float3(0, height, 0)),
		float3(instance.position + b2 * width + float3(0, height, 0))
	};
	
	float4 clipPoss[kMaxBladeVertexNum];
	for (int i = 0; i < kMaxBladeVertexNum; ++i) {
		clipPoss[i] = mul(float4(bladePoss[i], 1.0), viewProjection.matVP);
	}

	for (int i = 0; i < 5; ++i) {
		verts[i].position = clipPoss[i];
		verts[i].normal = float3(0, 1, 0);
		verts[i].uv = bladeUVs[i];
	}
	
	indis[0] = uint3(0, 1, 2);
	indis[1] = uint3(0, 3, 4);
	
}
