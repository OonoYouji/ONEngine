#include "Grass.hlsli"

#include "../../ConstantBufferData/ViewProjection.hlsli"

struct Time {
	float value;
};


ConstantBuffer<ViewProjection> viewProjection : register(b0);
StructuredBuffer<BladeInstance> bladeInstances : register(t0);
StructuredBuffer<Time> time : register(t1);

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(8, 8, 1)]
void MSMain(uint3 DTid : SV_DispatchThreadID,
			out vertices VertexOut verts[5],
			out indices uint3 indis[2]) {
	
	SetMeshOutputCounts(5, 2);
	
	uint index = DTid.x + DTid.y * 8; // 8x8のスレッドグループを想定
	
	BladeInstance instance = bladeInstances[index];
	
	float3 t = normalize(instance.tangent);
	float3 up = float3(0, 1, 0);
	//float3 b = normalize(cross(t, up)); // 横方向のベクトル
	float3 b = float3(1, 0, 0); // 横方向のベクトル
	float3 b2 = float3(0, 0, 1); // 横方向のベクトル
	float width = instance.scale;
	float height = 2.0f * instance.scale;
	
	
	float sinValue = sin(time[0].value + instance.random01 * 6.28);

	float3 p0 = instance.position + float3(0, height, 0) + (normalize(instance.tangent) * sinValue);
	float3 p1 = instance.position + b * -width;
	float3 p2 = instance.position + b * width;
	float3 p3 = instance.position + b2 * -width;
	float3 p4 = instance.position + b2 * width;

	float4 clipPos0 = mul(float4(p0, 1.0), viewProjection.matVP);
	float4 clipPos1 = mul(float4(p1, 1.0), viewProjection.matVP);
	float4 clipPos2 = mul(float4(p2, 1.0), viewProjection.matVP);
	float4 clipPos3 = mul(float4(p3, 1.0), viewProjection.matVP);
	float4 clipPos4 = mul(float4(p4, 1.0), viewProjection.matVP);
	
	verts[0].position = clipPos0;
	verts[1].position = clipPos1;
	verts[2].position = clipPos2;
	verts[3].position = clipPos3;
	verts[4].position = clipPos4;

	for (int i = 0; i < 5; ++i) {
		verts[i].normal = float3(0, 1, 0);
		verts[i].uv = float2(bladeVertices[i].x + 0.5, bladeVertices[i].y);
	}
	
	indis[0] = uint3(0, 1, 2);
	indis[1] = uint3(0, 3, 4);
	
}
