#include "VoxelTerrain.hlsli"
#include "../VoxelTerrainTest/Table.hlsli"

// ---------------------------------------------------
// Buffers (Tablesを削除しました)
// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

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

// 16頂点のcubeOffsets（各面ごとに4頂点ずつ）
static const float3 cubeOffsets[24] = {
    // Bottom (Y=-0.5)
    float3(-0.5, -0.5, -0.5),
    float3( 0.5, -0.5, -0.5),
    float3( 0.5, -0.5,  0.5),
    float3(-0.5, -0.5,  0.5),

    // Top (Y=+0.5)
    float3(-0.5, 0.5, -0.5),
    float3( 0.5, 0.5, -0.5),
    float3( 0.5, 0.5,  0.5),
    float3(-0.5, 0.5,  0.5),

    // Front (Z=+0.5)
    float3(-0.5, -0.5, 0.5),
    float3( 0.5, -0.5, 0.5),
    float3( 0.5,  0.5, 0.5),
    float3(-0.5,  0.5, 0.5),

    // Back (Z=-0.5)
    float3(-0.5, -0.5, -0.5),
    float3( 0.5, -0.5, -0.5),
    float3( 0.5,  0.5, -0.5),
    float3(-0.5,  0.5, -0.5),

    // Right (X=+0.5)
    float3(0.5, -0.5, -0.5),
    float3(0.5,  0.5, -0.5),
    float3(0.5,  0.5,  0.5),
    float3(0.5, -0.5,  0.5),

    // Left (X=-0.5)
    float3(-0.5, -0.5, -0.5),
    float3(-0.5,  0.5, -0.5),
    float3(-0.5,  0.5,  0.5),
    float3(-0.5, -0.5,  0.5)
};


// 面ごとの法線
static const float3 faceNormals[4] = {
    float3(0,-1,0),  // Bottom
    float3(0, 1,0),  // Top
    float3(0,0, 1),  // Front
    float3(0,0,-1)   // Back
};

// 各面2三角形、インデックス
static const uint3 faceIndices[6][2] = {
    { uint3(0, 1, 2), uint3(0, 2, 3) },    // Bottom  (0-3)
    { uint3(4, 6, 5), uint3(4, 7, 6) },    // Top     (4-7)
    { uint3(8, 9,10), uint3(8,10,11) },    // Front   (8-11)
    { uint3(12,14,13), uint3(12,15,14) },  // Back    (12-15)
    { uint3(16,17,18), uint3(16,18,19) },  // Right   (16-19)
    { uint3(20,23,22), uint3(20,22,21) }   // Left    (20-23)
};

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(2, 2, 2)]
void main(
	uint3 DTid : SV_DispatchThreadID,
	in payload Payload asPayload,
	out vertices VertexOut verts[256],
	out indices uint3 indis[256]) {

	uint3 step = asPayload.subChunkSize;
	float3 basePos = float3(DTid * step);

    uint cubeCount = 0;
	
    /// 
    if(asPayload.lodLevel != 0) {
        float3 samplePos = basePos;
        float d = GetDensity(samplePos + asPayload.chunkOrigin, asPayload.chunkIndex);
        if (d >= voxelTerrainInfo.isoLevel) {
            cubeCount += 1;
        }
    }

    uint cubeOffset = WavePrefixSum(cubeCount);
    uint totalCubeCount = WaveActiveSum(cubeCount);

    // GroupMemoryBarrier();
    SetMeshOutputCounts(totalCubeCount * 24, totalCubeCount * 12);
    if(totalCubeCount == 0) {
        return;
    }

    uint vOffset = cubeOffset * 24;
    uint iOffset = cubeOffset * 12;

    [unroll]
    for(uint32_t i = 0; i < 24; ++i) {
        float3 offset = cubeOffsets[i];
        float3 worldPos = basePos + (offset * float3(step)) + asPayload.chunkOrigin;

        verts[vOffset + i].worldPosition = float4(worldPos, 1.0f);
        verts[vOffset + i].position = mul(float4(worldPos, 1.0f), viewProjection.matVP);
        // verts[vOffset + i].color = DebugColor(asPayload.chunkIndex);
        verts[vOffset + i].normal = normalize(offset);
    }
    
    [unroll]
    for(uint32_t i = 0; i < 6; ++i) {
        indis[iOffset + (i*2 + 0)] = faceIndices[i][0] + vOffset;
        indis[iOffset + (i*2 + 1)] = faceIndices[i][1] + vOffset;
    }

}