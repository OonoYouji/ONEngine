
#include "VoxelTerrainCommon.hlsli"
#include "../VoxelTerrainTest/Table.hlsli"

struct Vertex {
    float3 position;
};

struct GenerateParams {
    uint32_t step; /// チャンクを分割するステップ数
};

struct ChunkID {
    uint32_t value;
};

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<GenerateParams> generateParams : register(b1);
ConstantBuffer<ChunkID> chunkID : register(b2); 

RWStructuredBuffer<Vertex> vertices : register(u0);
StructuredBuffer<Chunk> chunks : register(t1);
Texture3D<float4> chunkTextures[] : register(t2);
SamplerState samplerState : register(s0);


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
    
	return chunkTextures[chunks[chunkId].textureId].SampleLevel(samplerState, uvw, 0).a;
}

// 頂点補間
Vertex VertexInterp(float3 p1, float3 p2, float3 chunkOrigin,float3 subChunkSize, float d1, float d2) {
	Vertex vOut;
	
	// 補間係数の計算（ゼロ除算対策）
	float denom = d2 - d1;
	float t = 0.5f; // デフォルト値
	
	if (abs(denom) > 0.00001f) {
		t = (voxelTerrainInfo.isoLevel - d1) / denom;
		t = saturate(t); // [0,1]にクランプ
	}
	
	float3 localPos = lerp(p1, p2, t);
	
	vOut.position = localPos + chunkOrigin;
    vOut.position.xz += (subChunkSize / 2.0).xz; 

	return vOut;
}

float32_t3 GetBasePos(uint32_t id, uint32_t3 size, uint32_t3 step) {
    uint32_t3 gridPos = uint32_t3(
        id % size.x,
        (id / size.x) % size.y,
        id / (size.x * size.y)
    );
    return float32_t3(gridPos * step);
}


[shader("compute")]
[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {

    /// チャンク内のボクセル数を超えたらもう処理の必要がない
    uint32_t totalChunkVoxelCount = voxelTerrainInfo.chunkSize.x * voxelTerrainInfo.chunkSize.y * voxelTerrainInfo.chunkSize.z;
    if(DTid.x >= totalChunkVoxelCount) {
        return;
    }

	uint32_t3 step = uint32_t3(generateParams.step, generateParams.step, generateParams.step);
	float32_t3 basePos = GetBasePos(DTid.x, voxelTerrainInfo.chunkSize, step);


    float32_t cubeDensities[8];
    uint32_t cubeIndex = 0;
    [unroll]
    for(int i=0; i<8; ++i) {
        float32_t3 samplePos = basePos + (kCornerOffsets[i] * float32_t3(step));
        float d = GetDensity(samplePos, DTid.y);
        cubeDensities[i] = d;

        if(d < voxelTerrainInfo.isoLevel) {
            cubeIndex |= (1u << i);
        }
    }

    uint32_t triCount = 0;
    [unroll]
    for(int i=0; i<5; i++) {
        triCount += (TriTable[cubeIndex][i * 3] != -1) ? 1 : 0;
    }

    
    uint outputTriOffset = WavePrefixSum(triCount);
    uint totalTriCount = WaveActiveSum(triCount);

	/// チャンクの原点を計算
	float3 chunkOrigin = float3(
		(chunkID.value % voxelTerrainInfo.chunkCountXZ.x) * voxelTerrainInfo.chunkSize.x,
		0,
		(chunkID.value / voxelTerrainInfo.chunkCountXZ.x) * voxelTerrainInfo.chunkSize.z
	);


	for (uint t = 0; t < triCount; t++) {
        uint currentTriIndex = outputTriOffset + t;
        uint vIndex = currentTriIndex * 3;

		for (int v = 0; v < 3; v++) {
			int edgeIndex = TriTable[cubeIndex][(t * 3) + v];
			
			int idx1 = kEdgeConnection[edgeIndex].x;
			int idx2 = kEdgeConnection[edgeIndex].y;
			
			float3 p1 = basePos + (kCornerOffsets[idx1] * float3(step));
			float3 p2 = basePos + (kCornerOffsets[idx2] * float3(step));
			
			vertices[vIndex + v] = VertexInterp(p1, p2, chunkOrigin, float32_t3(step), cubeDensities[idx1], cubeDensities[idx2]);
		}
    }

}