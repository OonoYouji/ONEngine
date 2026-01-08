#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"

// ---------------------------------------------------
// Buffers (Tablesを削除しました)
// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);


// マーチングキューブ用の定数定義
static const float kIsoLevel = 0.5f;

// ---------------------------------------------------
// Constants
// ---------------------------------------------------

// 頂点オフセット (0-7) - 中心を原点とする版
static const float3 kCornerOffsets[8] = {
	float3(-0.5, -0.5, -0.5), float3(0.5, -0.5, -0.5),
	float3(0.5, 0.5, -0.5), float3(-0.5, 0.5, -0.5),
	float3(-0.5, -0.5, 0.5), float3(0.5, -0.5, 0.5),
	float3(0.5, 0.5, 0.5), float3(-0.5, 0.5, 0.5)
};

// エッジ接続情報
static const int2 kEdgeConnection[12] = {
	int2(0, 1), int2(1, 2), int2(2, 3), int2(3, 0), // Bottom
	int2(4, 5), int2(5, 6), int2(6, 7), int2(7, 4), // Top
	int2(0, 4), int2(1, 5), int2(2, 6), int2(3, 7) // Vertical
};

// ---------------------------------------------------
// Functions
// ---------------------------------------------------

// 密度取得
float GetDensity(float3 _localPos, uint _chunkId) {
	float voxelSize = 1.0f;
    
    // テクスチャサイズを float3 として取得
	float3 textureSize = float3(voxelTerrainInfo.textureSize);
    
    // ローカル座標をUVW座標に変換（各軸で異なるサイズを使用）
	float3 uvw = _localPos / (textureSize * voxelSize);
	uvw.y = 1.0f - uvw.y;
    
	uint chunkId = _chunkId;
    
    // チャンクのグリッド座標を計算
	int chunkX = int(chunkId) % int(voxelTerrainInfo.chunkCountXZ.x);
	int chunkZ = int(chunkId) / int(voxelTerrainInfo.chunkCountXZ.x);
    
    // X方向の境界処理
	if (uvw.x < 0.0f) {
		if (chunkX > 0) {
			chunkId = _chunkId - 1;
			uvw.x += 1.0f;
		} else {
			return 0.0f;
		}
	} else if (uvw.x > 1.0f) {
		if (chunkX < int(voxelTerrainInfo.chunkCountXZ.x) - 1) {
			chunkId = _chunkId + 1;
			uvw.x -= 1.0f;
		} else {
			return 0.0f;
		}
	}
    
    // Z方向の境界処理
	if (uvw.z < 0.0f) {
		if (chunkZ > 0) {
			chunkId -= uint(voxelTerrainInfo.chunkCountXZ.x);
			uvw.z += 1.0f;
		} else {
			return 0.0f;
		}
	} else if (uvw.z > 1.0f) {
		if (chunkZ < int(voxelTerrainInfo.chunkCountXZ.y) - 1) {
			chunkId += uint(voxelTerrainInfo.chunkCountXZ.x);
			uvw.z -= 1.0f;
		} else {
			return 0.0f;
		}
	}
    
    // Y方向の境界処理
	if (uvw.y < 0.0f || uvw.y > 1.0f) {
		return 1.0f;
	}
    
    // UVWをクランプして安全性を確保
	uvw = saturate(uvw);
    
	return voxelChunkTextures[chunks[chunkId].textureId].SampleLevel(texSampler, uvw, 0).a;
}

// ---------------------------------------------------
// 密度サンプリング（コーナーと追加境界サンプル）
// ---------------------------------------------------
void SampleDensities(
    uint3 basePos,
    uint step,
    uint myLOD,
    uint neighborLOD,
    out float densities[13]) {
    // まずコーナー8点
    [unroll]
	for (int i = 0; i < 8; ++i) {
		float3 cornerOffset = kCornerOffsets[i] * step;
		densities[i] = GetDensity(basePos + cornerOffset, myLOD);
	}

    // Transvoxel用の追加サンプル（境界補間用）
    // 仮に簡単に中央点4点を追加
	densities[8] = GetDensity(basePos + float3(step * 0.5, 0, 0), myLOD);
	densities[9] = GetDensity(basePos + float3(0, step * 0.5, 0), myLOD);
	densities[10] = GetDensity(basePos + float3(0, 0, step * 0.5), myLOD);
	densities[11] = GetDensity(basePos + float3(step * 0.5, 0, step * 0.5), myLOD);
	densities[12] = GetDensity(basePos + float3(0, step * 0.5, step * 0.5), myLOD);
}

// ---------------------------------------------------
// Transvoxelケースインデックス計算
// ---------------------------------------------------
uint ComputeTransvoxelCase(float densities[13]) {
	uint caseIndex = 0;
    [unroll]
	for (int i = 0; i < 8; ++i) {
		if (densities[i] < kIsoLevel)
			caseIndex |= (1u << i);
	}
	return caseIndex;
}

// ---------------------------------------------------
// 補間した頂点座標計算
// ---------------------------------------------------
float3 InterpolateTransvoxelVertex(
    uint vertexIndex,
    float densities[13],
    uint3 basePos,
    uint step) {
    // 仮：コーナー間線形補間
	int idx0 = kEdgeConnection[vertexIndex].x;
	int idx1 = kEdgeConnection[vertexIndex].y;

	float d0 = densities[idx0];
	float d1 = densities[idx1];

	float t = 0.5f;
	float denom = d1 - d0;
	if (abs(denom) > 0.00001f)
		t = (kIsoLevel - d0) / denom;

	t = saturate(t);

	float3 p0 = basePos + kCornerOffsets[idx0] * step;
	float3 p1 = basePos + kCornerOffsets[idx1] * step;

	return lerp(p0, p1, t);
}

// ---------------------------------------------------
// 法線計算（中央差分）
// ---------------------------------------------------
float3 ComputeGradient(float3 pos) {
	float step = 0.5f;
	float dx = GetDensity(pos + float3(step, 0, 0), 0) - GetDensity(pos - float3(step, 0, 0), 0);
	float dy = GetDensity(pos + float3(0, step, 0), 0) - GetDensity(pos - float3(0, step, 0), 0);
	float dz = GetDensity(pos + float3(0, 0, step), 0) - GetDensity(pos - float3(0, 0, step), 0);

	float3 grad = float3(dx, dy, dz);
	float len = length(grad);
	if (len < 1e-5f)
		return float3(0, 1, 0);
	return normalize(-grad);
}

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void MS_Transvoxel(
    uint3 DTid : SV_DispatchThreadID,
    uint gi : SV_GroupIndex,
    in Payload payload,
    out VertexOut verts[16],
    out uint3 indis[6]) {
	uint face = payload.face;
	uint step = 1u << payload.myLOD;
	uint cellIdxX = DTid.x;
	uint cellIdxY = DTid.y;

    // 境界セルの基点計算（例：XP面）
	uint3 basePos = payload.chunkOrigin;
	switch (face) {
		case 0: // XP
			basePos.x += voxelTerrainInfo.chunkSize.x - step;
			basePos.y += cellIdxY * step;
			basePos.z += cellIdxX * step;
			break;
		case 1: // XN
			basePos.x += 0;
			basePos.y += cellIdxY * step;
			basePos.z += cellIdxX * step;
			break;
		case 2: // ZP
			basePos.z += voxelTerrainInfo.chunkSize.z - step;
			basePos.x += cellIdxX * step;
			basePos.y += cellIdxY * step;
			break;
		case 3: // ZN
			basePos.z += 0;
			basePos.x += cellIdxX * step;
			basePos.y += cellIdxY * step;
			break;
		case 4: // YP
			basePos.y += voxelTerrainInfo.chunkSize.y - step;
			basePos.x += cellIdxX * step;
			basePos.z += cellIdxY * step;
			break;
		case 5: // YN
			basePos.y += 0;
			basePos.x += cellIdxX * step;
			basePos.z += cellIdxY * step;
			break;
	}

    // ここでTransvoxelのケースを計算
    // corner density取得
	float densities[13]; // Transvoxelは追加サンプルを持つ
	SampleDensities(basePos, step, payload.myLOD, payload.neighborLOD, densities);

    // ケースインデックス
	uint caseIndex = ComputeTransvoxelCase(densities);

    // テーブルルックアップ
	uint triCount = TransitionTriangleCount[caseIndex];

    [unroll]
	for (uint t = 0; t < triCount; ++t) {
		uint3 indices = TransitionTriangles[caseIndex][t];
        // 各頂点の補間
        [unroll]
		for (uint v = 0; v < 3; ++v) {
			float3 p = InterpolateTransvoxelVertex(indices[v], densities, basePos, step);
			verts[t * 3 + v].worldPos = float4(p, 1);
			verts[t * 3 + v].position = mul(verts[t * 3 + v].worldPos, viewProjection.matVP);
			verts[t * 3 + v].normal = ComputeGradient(p); // 法線
			//verts[t * 3 + v].color = float4(0.3, 0.8, 0.3, 1); // 仮
		}

        // インデックス
		indis[t] = uint3(t * 3 + 0, t * 3 + 1, t * 3 + 2);
	}
}