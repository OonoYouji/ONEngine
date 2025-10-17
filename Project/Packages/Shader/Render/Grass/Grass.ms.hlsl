//#include "Grass.hlsli"
#include "BladeInstance.hlsli"


struct Time {
	float value;
};

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

	uint threadIndex = DTid.x;
	uint baseIndex = asPayload.startIndices[threadIndex];

    // 一時バッファ：可視と判定した草の gi 値を保存
	uint visibleIds[kMaxRenderingGrassSize];
	uint visibleCount = 0;

    // ---- パス1：可視判定だけ（頂点/インデックスは書かない） ----
	for (int gi = 0; gi < kMaxRenderingGrassSize; ++gi) {
		uint grassIndex = baseIndex + gi;
		BladeInstance instance = bladeInstances[grassIndex];

        // 位置計算（あなたの既存ロジック）
		float3 t = normalize(instance.tangent);
		float3 up = float3(0, 1, 0);
		float3 b = normalize(cross(t, up));
		float3 b2 = normalize(cross(b, up));
		float width = instance.scale;
		float height = 2.0f * instance.scale;

		float3 bladePoss[kMaxBladeVertexNum] = {
			float3(instance.position - float3(0, height, 0)),
            float3(instance.position + b * -width + float3(0, height, 0)),
            float3(instance.position + b * width + float3(0, height, 0)),
            float3(instance.position + b2 * -width + float3(0, height, 0)),
            float3(instance.position + b2 * width + float3(0, height, 0))
		};

        // 揺れの計算（UAVへの書き込みはここでしてもOKだが、必要なら二段目に移す）
		time[grassIndex].value += 1.0f / 60.0f;
		float sinValue = sin(time[grassIndex].value + instance.random01 * 6.28);
		for (int i = 1; i < kMaxBladeVertexNum; ++i) {
			bladePoss[i] += (normalize(instance.tangent) * sinValue * 0.1);
		}

        // クリップ判定
		bool isInside = true;
		for (int i = 0; i < kMaxBladeVertexNum; ++i) {
			float4 clip = mul(float4(bladePoss[i], 1.0), viewProjection.matVP);
			float3 ndc = clip.xyz / clip.w;
			if (ndc.x < -1 || ndc.x > 1 || ndc.y < -1 || ndc.y > 1 || ndc.z < 0 || ndc.z > 1) {
				isInside = false;
				break;
			}
		}

		if (isInside) {
			visibleIds[visibleCount++] = gi; // giを保存
		}
	}

    // ---- ここで全ての出力はまだ発生していない ----
    // (numthreadsが1ならGroupMemoryBarrierは不要だが、他のスレッドがある場合は同期を入れる)
	SetMeshOutputCounts(visibleCount * kMaxBladeVertexNum, visibleCount * 2);

    // ---- パス2：確定した可視リストを使って頂点/インデックスを書き込む ----
	for (uint vi = 0; vi < visibleCount; ++vi) {
		uint gi = visibleIds[vi];
		uint grassIndex = baseIndex + gi;
		BladeInstance instance = bladeInstances[grassIndex];

        // --- 再度bladePossを計算（またはパス1で保存しておく） ---
		float3 t = normalize(instance.tangent);
		float3 up = float3(0, 1, 0);
		float3 b = normalize(cross(t, up));
		float3 b2 = normalize(cross(b, up));
		float width = instance.scale;
		float height = 2.0f * instance.scale;

		float3 bladePoss[kMaxBladeVertexNum] = {
			float3(instance.position - float3(0, height, 0)),
            float3(instance.position + b * -width + float3(0, height, 0)),
            float3(instance.position + b * width + float3(0, height, 0)),
            float3(instance.position + b2 * -width + float3(0, height, 0)),
            float3(instance.position + b2 * width + float3(0, height, 0))
		};

		float sinValue = sin(time[grassIndex].value + instance.random01 * 6.28);
		for (int i = 1; i < kMaxBladeVertexNum; ++i) {
			bladePoss[i] += (normalize(instance.tangent) * sinValue * 0.1);
		}

        // クリップ座標に変換して書き込み
		uint startVertIndex = vi * kMaxBladeVertexNum;
		uint startIndiIndex = vi * 2;

		for (int i = 0; i < kMaxBladeVertexNum; ++i) {
			float4 clip = mul(float4(bladePoss[i], 1.0), viewProjection.matVP);
			verts[startVertIndex + i].position = clip;
			verts[startVertIndex + i].normal = float3(0, 1, 0);
			verts[startVertIndex + i].uv = bladeUVs[i];
		}

		indis[startIndiIndex + 0] = uint3(startVertIndex + 0, startVertIndex + 1, startVertIndex + 2);
		indis[startIndiIndex + 1] = uint3(startVertIndex + 0, startVertIndex + 3, startVertIndex + 4);
	}
	
}
