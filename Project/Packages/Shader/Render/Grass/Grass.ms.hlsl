//#include "Grass.hlsli"
#include "BladeInstance.hlsli"

#include "../../ConstantBufferData/ViewProjection.hlsli"

struct Time {
	float value;
};

ConstantBuffer<ViewProjection> viewProjection : register(b0);
RWStructuredBuffer<Time> time : register(u0);

/*
 * 一個の草の頂点数
 * SetMeshOutputCounts(5, 2);
 * これを一回のms内で出来る限り描画する
 * 256/5 = 51
 * 51*2 = 102
 * 51本の草を一回のmsで描画できる
 */

/// 一度のmsで処理する草の数

/// 一個の草をverts, indsに出力する関数を呼び出す

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void MSMain(uint3 DTid : SV_DispatchThreadID,
			uint3 Gid : SV_GroupThreadID,
			uint gIndex : SV_GroupIndex,
			in payload Payload asPayload,
			out vertices VertexOut verts[51 * 5],
			out indices uint3 indis[51 * 2]) {

	/// 出来る限り多くの頂点を出力する
	SetMeshOutputCounts(51 * 5, 51 * 2);
	
	/// 草の数分だけ処理する
	for (int gi = 0; gi < kMaxRenderingGrassSize; ++gi) {
		
		uint startVertIndex = gi * kMaxBladeVertexNum;
		uint startIndiIndex = gi * 2;
		
		uint bladeIndex = asPayload.grassData[gi].index;
		BladeInstance instance = bladeInstances[bladeIndex];
		
		
		float3 t = normalize(instance.tangent);
		float3 up = float3(0, 1, 0);
		float3 b = normalize(cross(t, up)); // 横方向のベクトル
		float3 b2 = normalize(cross(b, up)); // 横方向のベクトル
		float width = instance.scale;
		float height = 2.0f * instance.scale;
	
	
		float3 bladePoss[kMaxBladeVertexNum] = {
			/// 根本頂点
			float3(instance.position - float3(0, height, 0)),
			/// 他、上部の頂点
			float3(instance.position + b * -width + float3(0, height, 0)),
			float3(instance.position + b * width + float3(0, height, 0)),
			float3(instance.position + b2 * -width + float3(0, height, 0)),
			float3(instance.position + b2 * width + float3(0, height, 0))
		};
	

		/// 揺らす (根本以外)
		time[bladeIndex].value += 1.0f / 60.0f;
		float sinValue = sin(time[bladeIndex].value + instance.random01 * 6.28);
		for (int i = 1; i < kMaxBladeVertexNum; ++i) {
			float3 newPos = bladePoss[i];
			newPos += (normalize(instance.tangent) * sinValue * 0.1);
			bladePoss[i] = newPos;
		}


		float4 clipPoss[kMaxBladeVertexNum];
		for (int i = 0; i < kMaxBladeVertexNum; ++i) {
			clipPoss[i] = mul(float4(bladePoss[i], 1.0), viewProjection.matVP);
		}

		for (int i = 0; i < 5; ++i) {
			verts[startVertIndex + i].position = clipPoss[i];
			verts[startVertIndex + i].normal = float3(0, 1, 0);
			verts[startVertIndex + i].uv = bladeUVs[i];
		}
		
		indis[startIndiIndex + 0] = uint3(startVertIndex + 0, startVertIndex + 1, startVertIndex + 2);
		indis[startIndiIndex + 1] = uint3(startVertIndex + 0, startVertIndex + 3, startVertIndex + 4);
	}
	
}
