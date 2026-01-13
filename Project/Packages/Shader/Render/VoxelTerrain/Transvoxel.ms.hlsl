#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"

// ---------------------------------------------------
// Buffers
// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

// マーチングキューブ用の定数定義
static const float kIsoLevel = 0.5f;

// ---------------------------------------------------
// Constants
// ---------------------------------------------------

static const float3 kCornerOffsets[8] = {
	float3(-0.5, -0.5, -0.5), float3(0.5, -0.5, -0.5),
    float3(0.5, 0.5, -0.5), float3(-0.5, 0.5, -0.5),
    float3(-0.5, -0.5, 0.5), float3(0.5, -0.5, 0.5),
    float3(0.5, 0.5, 0.5), float3(-0.5, 0.5, 0.5)
};

static const int2 kEdgeConnection[12] = {
	int2(0, 1), int2(1, 2), int2(2, 3), int2(3, 0), // Bottom
    int2(4, 5), int2(5, 6), int2(6, 7), int2(7, 4), // Top
    int2(0, 4), int2(1, 5), int2(2, 6), int2(3, 7) // Vertical
};

// ---------------------------------------------------
// Functions (変更なし)
// ---------------------------------------------------

float GetDensity(float3 _localPos, uint _chunkId) {
	float voxelSize = 1.0f;
	float3 textureSize = float3(voxelTerrainInfo.textureSize);
	float3 uvw = _localPos / (textureSize * voxelSize);
	uvw.y = 1.0f - uvw.y;
    
	uint chunkId = _chunkId;
	int chunkX = int(chunkId) % int(voxelTerrainInfo.chunkCountXZ.x);
	int chunkZ = int(chunkId) / int(voxelTerrainInfo.chunkCountXZ.x);

	if (uvw.x < 0.0f) {
		if (chunkX > 0) {
			chunkId = _chunkId - 1;
			uvw.x += 1.0f;
		} else
			return 0.0f;
	} else if (uvw.x > 1.0f) {
		if (chunkX < int(voxelTerrainInfo.chunkCountXZ.x) - 1) {
			chunkId = _chunkId + 1;
			uvw.x -= 1.0f;
		} else
			return 0.0f;
	}

	if (uvw.z < 0.0f) {
		if (chunkZ > 0) {
			chunkId -= uint(voxelTerrainInfo.chunkCountXZ.x);
			uvw.z += 1.0f;
		} else
			return 0.0f;
	} else if (uvw.z > 1.0f) {
		if (chunkZ < int(voxelTerrainInfo.chunkCountXZ.y) - 1) {
			chunkId += uint(voxelTerrainInfo.chunkCountXZ.x);
			uvw.z -= 1.0f;
		} else
			return 0.0f;
	}

	if (uvw.y < 0.0f || uvw.y > 1.0f)
		return 1.0f;
	uvw = saturate(uvw);
	return voxelChunkTextures[chunks[chunkId].textureId].SampleLevel(texSampler, uvw, 0).a;
}

void SampleDensities(uint3 basePos, uint step, uint myLOD, uint neighborLOD, out float densities[13]) {
    [unroll]
	for (int i = 0; i < 8; ++i) {
		float3 cornerOffset = kCornerOffsets[i] * step;
		densities[i] = GetDensity(basePos + cornerOffset, myLOD);
	}
    // Transvoxel追加サンプル
	densities[8] = GetDensity(basePos + float3(step * 0.5, 0, 0), myLOD);
	densities[9] = GetDensity(basePos + float3(0, step * 0.5, 0), myLOD);
	densities[10] = GetDensity(basePos + float3(0, 0, step * 0.5), myLOD);
	densities[11] = GetDensity(basePos + float3(step * 0.5, 0, step * 0.5), myLOD);
	densities[12] = GetDensity(basePos + float3(0, step * 0.5, step * 0.5), myLOD);
}

uint ComputeTransvoxelCase(float densities[13]) {
	uint caseIndex = 0;
    [unroll]
	for (int i = 0; i < 8; ++i) {
		if (densities[i] < kIsoLevel)
			caseIndex |= (1u << i);
	}
	return caseIndex;
}

float3 InterpolateTransvoxelVertex(uint vertexIndex, float densities[13], uint3 basePos, uint step) {
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

float3 ComputeGradient(float3 pos) {
	float step = 0.5f;
	float dx = GetDensity(pos + float3(step, 0, 0), 0) - GetDensity(pos - float3(step, 0, 0), 0);
	float dy = GetDensity(pos + float3(0, step, 0), 0) - GetDensity(pos - float3(0, step, 0), 0);
	float dz = GetDensity(pos + float3(0, 0, step), 0) - GetDensity(pos - float3(0, 0, step), 0);
	float3 grad = float3(dx, dy, dz);
	return (length(grad) < 1e-5f) ? float3(0, 1, 0) : normalize(-grad);
}

// ---------------------------------------------------
// Main Shader Function
// ---------------------------------------------------
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)] // 1スレッドで1ボクセルを処理する構成
void main(
    uint3 DTid : SV_DispatchThreadID,
    uint gi : SV_GroupIndex,
    in payload Payload asPayload,
    out vertices TransvoxelVertexOut verts[16], // 最大出力数
    out indices uint3 indis[6]) {

	uint face = asPayload.face;
	uint step = 1u << asPayload.myLOD;
	uint cellIdxX = DTid.x;
	uint cellIdxY = DTid.y;

    // 1. 基点計算
	uint3 basePos = asPayload.chunkOrigin;
	switch (face) {
		case 0:
			basePos.x += voxelTerrainInfo.chunkSize.x - step;
			basePos.y += cellIdxY * step;
			basePos.z += cellIdxX * step;
			break; // XP
		case 1:
			basePos.x += 0;
			basePos.y += cellIdxY * step;
			basePos.z += cellIdxX * step;
			break; // XN
		case 2:
			basePos.z += voxelTerrainInfo.chunkSize.z - step;
			basePos.x += cellIdxX * step;
			basePos.y += cellIdxY * step;
			break; // ZP
		case 3:
			basePos.z += 0;
			basePos.x += cellIdxX * step;
			basePos.y += cellIdxY * step;
			break; // ZN
		case 4:
			basePos.y += voxelTerrainInfo.chunkSize.y - step;
			basePos.x += cellIdxX * step;
			basePos.z += cellIdxY * step;
			break; // YP
		case 5:
			basePos.y += 0;
			basePos.x += cellIdxX * step;
			basePos.z += cellIdxY * step;
			break; // YN
	}

    // 2. 密度とケース計算
	float densities[13];
	SampleDensities(basePos, step, asPayload.myLOD, asPayload.neighborLOD, densities);
	uint caseIndex = ComputeTransvoxelCase(densities);

    // 3. 三角形数の取得
    // テーブルのTransitionTriangleCountは static const int 配列として定義されている前提
	uint triCount = TransitionTriangleCount[caseIndex];
	SetMeshOutputCounts(triCount * 3, triCount);

    // データがない場合はここで終了してOK
	if (triCount == 0) {
		return;
	}


    // 4. 頂点とプリミティブの生成
	for (uint t = 0; t < triCount; ++t) {
        
        // テーブルからインデックス取得
		int i0 = TransitionTriangles[caseIndex][t * 3 + 0];
		int i1 = TransitionTriangles[caseIndex][t * 3 + 1];
		int i2 = TransitionTriangles[caseIndex][t * 3 + 2];

        // センチネルチェック (-1 ならデータなし)
		if (i0 < 0 || i1 < 0 || i2 < 0) {
			break;
		}

		uint3 indices = uint3(i0, i1, i2);

        // 各頂点の計算と書き込み
        [unroll]
		for (uint v = 0; v < 3; ++v) {
            // 書き込み先のインデックス計算
			uint outIndex = t * 3 + v;

			float3 p = InterpolateTransvoxelVertex(indices[v], densities, basePos, step);
            
			verts[outIndex].worldPos = float4(p, 1);
			verts[outIndex].position = mul(verts[outIndex].worldPos, viewProjection.matVP);
			verts[outIndex].normal = ComputeGradient(p);
		}

        // プリミティブ（三角形）インデックスの書き込み
		indis[t] = uint3(t * 3 + 0, t * 3 + 1, t * 3 + 2);
	}
}