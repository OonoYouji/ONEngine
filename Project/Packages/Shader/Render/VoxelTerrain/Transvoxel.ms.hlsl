#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"
#include "../VoxelTerrainTest/Table.hlsli"

// -----------------------------------------------------------------------------
// Resources
// -----------------------------------------------------------------------------
Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);


float32_t3 GetChunkPos(uint32_t chunkID) {
    uint32_t x = chunkID % voxelTerrainInfo.chunkCountXZ.x;
    uint32_t z = chunkID / voxelTerrainInfo.chunkCountXZ.x;
    
    return float32_t3(x, 0, z) * float32_t3(voxelTerrainInfo.chunkSize) + float32_t3(voxelTerrainInfo.terrainOrigin);
}

float32_t3 GetChunkCenter(uint32_t chunkID) {
    float32_t3 chunkPos = GetChunkPos(chunkID);
    return chunkPos + float32_t3(voxelTerrainInfo.chunkSize) / 2;
}

float32_t GetDensity(float32_t3 samplePos, uint32_t chunkID) {
	float voxelSize = 1.0f;
    float32_t3 textureSize = float32_t3(voxelTerrainInfo.textureSize);
    float32_t3 uvw = samplePos / (textureSize * voxelSize);

    /// Y軸反転
    uvw.y = 1.0f - uvw.y;

    // チャンクのグリッド座標を計算
	int chunkX = int(chunkID) % int(voxelTerrainInfo.chunkCountXZ.x);
	int chunkZ = int(chunkID) / int(voxelTerrainInfo.chunkCountXZ.x);

    uint32_t idx = chunkID;
    
    // X方向の境界処理
	if (uvw.x < 0.0f) {
		if (chunkX > 0) {
			idx = chunkID - 1;
			uvw.x += 1.0f;
		} else {
			return 0.0f;
		}
	} else if (uvw.x > 1.0f) {
		if (chunkX < int(voxelTerrainInfo.chunkCountXZ.x) - 1) {
			idx = chunkID + 1;
			uvw.x -= 1.0f;
		} else {
			return 0.0f;
		}
	}
    
    // Z方向の境界処理
	if (uvw.z < 0.0f) {
		if (chunkZ > 0) {
			idx -= uint(voxelTerrainInfo.chunkCountXZ.x);
			uvw.z += 1.0f;
		} else {
			return 0.0f;
		}
	} else if (uvw.z > 1.0f) {
		if (chunkZ < int(voxelTerrainInfo.chunkCountXZ.y) - 1) {
			idx += uint(voxelTerrainInfo.chunkCountXZ.x);
			uvw.z -= 1.0f;
		} else {
			return 0.0f;
		}
	}

	if (uvw.y < 0.0f || uvw.y > 1.0f) {
		return 1.0f;
	}

    uvw = saturate(uvw);

    float32_t4 rgba = voxelChunkTextures[chunks[idx].textureId].SampleLevel(texSampler, uvw, 0);
    return rgba.a;
}

float3 GetDebugColor(uint ID) {
    // 適当なハッシュ計算
    uint r = (ID * 123456789u + 12345u) % 256u;
    uint g = (ID * 987654321u + 54321u) % 256u;
    uint b = (ID * 192837465u + 13579u) % 256u;
    return float3(r, g, b) / 255.0f;
}

uint32_t GetSubChunkSize(uint32_t lodLevel) {
    return 2u << (lodLevel);
}

// 頂点補間
VertexOut VertexInterp(float3 p1, float3 p2, float3 _chunkOrigin,float3 subChunkSize, float d1, float d2, uint _chunkId) {
	VertexOut vOut;
	
	// 補間係数の計算（ゼロ除算対策）
	float denom = d2 - d1;
	float t = 0.5f; // デフォルト値
	
	if (abs(denom) > 0.00001f) {
		t = (voxelTerrainInfo.isoLevel - d1) / denom;
		t = saturate(t); // [0,1]にクランプ
	}
	
	float3 localPos = lerp(p1, p2, t);
	
	vOut.worldPosition = float4(localPos + _chunkOrigin, 1.0f);
    vOut.worldPosition.xz += (subChunkSize / 2.0).xz; 

	vOut.position = mul(vOut.worldPosition, viewProjection.matVP);
	
	// 補間位置での法線を計算
	// vOut.normal = GetGradient(localPos, _chunkId);
	
	// 高さベースの色付け
	float worldY = vOut.worldPosition.y;
	vOut.color = lerp(float4(0.3, 0.8, 0.3, 1), float4(0.6, 0.5, 0.3, 1), worldY / 512.0f);
	
	return vOut;
}


float3 GetNormal(float3 _p0, float3 _p1, float3 _p2) {
	float3 u = _p1 - _p0;
	float3 v = _p2 - _p0;
	return normalize(cross(u, v));
}


static const float32_t4 colors[8] = {
    float32_t4(1, 0, 0, 1),
    float32_t4(0, 1, 0, 1),
    float32_t4(0, 0, 1, 1),
    float32_t4(1, 1, 0, 1),
    float32_t4(1, 0, 1, 1),
    float32_t4(0, 1, 1, 1),
    float32_t4(1, 1, 1, 1),
    float32_t4(0.5, 0.5, 0.5, 1)
};


[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
    uint32_t3 DTid : SV_DispatchThreadID,
    uint32_t gtid : SV_GroupThreadID,
    uint32_t3 gid : SV_GroupID,
    in payload Payload payload,
    out vertices VertexOut verts[256],
    out indices uint32_t3 tris[256]) {
    
    // bool isVisible = true;

    uint32_t chunkID = payload.chunkID;
    // uint32_t lodLevel = payload.LODLevel;
    // uint32_t transitionMask = payload.transitionMask;

    float32_t3 chunkOrigin = payload.chunkOrigin;

    uint3 step = payload.subChunkSize;
	float3 basePos = float3(DTid * step);
    
 
    /// -----------------------------------------------------------------------------
    /// 生成するメッシュのサイズを計算する
    /// -----------------------------------------------------------------------------


	float cubeDensities[8];
    uint32_t cubeIndex = 0;
    
    [unroll]
    for(uint32_t i=0; i<8; ++i) {
        float32_t3 samplePos = basePos + (kCornerOffsets[i] * float32_t3(step));

        /// チャンク内のローカルに合わせる
        float32_t density = GetDensity(samplePos, chunkID);
        cubeDensities[i] = density;

        if(density < voxelTerrainInfo.isoLevel) {
            cubeIndex |= (1u << i);
        }
    }


    uint triCount = 0;
	[unroll]
	for (int i = 0; i < 15; i += 3) {
		triCount += (TriTable[cubeIndex][i] != -1) ? 1 : 0;
	}

    uint vertexOffset = 0;
    uint primitiveOffset = 0;

    SetMeshOutputCounts(triCount * 3, triCount);
	
	for (uint t = 0; t < triCount; t++) {
        uint vIndex = vertexOffset;
        uint pIndex = primitiveOffset;

		VertexOut outVerts[3];

		for (int v = 0; v < 3; v++) {
			int edgeIndex = TriTable[cubeIndex][(t * 3) + v];
			
			int idx1 = kEdgeConnection[edgeIndex].x;
			int idx2 = kEdgeConnection[edgeIndex].y;
			
			float3 p1 = basePos + (kCornerOffsets[idx1] * float3(step));
			float3 p2 = basePos + (kCornerOffsets[idx2] * float3(step));
			
			outVerts[v] = VertexInterp(p1, p2, payload.chunkOrigin, float32_t3(payload.subChunkSize), cubeDensities[idx1], cubeDensities[idx2], chunkID);
			verts[vIndex + (t * 3) + v] = outVerts[v];
		}
		
		float3 normal = GetNormal(
			outVerts[0].worldPosition.xyz,
			outVerts[1].worldPosition.xyz,
			outVerts[2].worldPosition.xyz
		);
		
		verts[vIndex + (t * 3) + 0].normal = normal;
		verts[vIndex + (t * 3) + 1].normal = normal;
		verts[vIndex + (t * 3) + 2].normal = normal;
		
		tris[pIndex + t] = uint3(
			vIndex + (t * 3) + 0,
			vIndex + (t * 3) + 1,
			vIndex + (t * 3) + 2
		);
	}

}