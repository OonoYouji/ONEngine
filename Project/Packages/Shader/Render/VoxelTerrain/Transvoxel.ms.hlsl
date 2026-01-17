#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"

// -----------------------------------------------------------------------------
// Resources
// -----------------------------------------------------------------------------
Texture3D<float> voxelChunkTextures[] : register(t1);
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
    float32_t3 textureSize = float32_t3(voxelTerrainInfo.textureSize);
    float32_t3 uvw = samplePos / textureSize;

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




groupshared uint sVertexCount;
groupshared uint sPrimitiveCount;
groupshared uint sVertexOffsets[64];    
groupshared uint sPrimitiveOffsets[64]; 


[shader("mesh")]
[outputtopology("triangle")]
[numthreads(64, 1, 1)]
void main(
    uint32_t gtid : SV_GroupThreadID,
    uint32_t3 gid : SV_GroupID,
    in payload Payload payload,
    out vertices VertexOut verts[256],
    out indices uint32_t3 tris[256]) {
    
    if (gtid == 0) {
        sVertexCount = 0;
        sPrimitiveCount = 0;
    }

    GroupMemoryBarrierWithGroupSync();

    /// -----------------------------------------------------------------------------
    /// 必要なデータを集める
    /// -----------------------------------------------------------------------------

    bool isVisible = true;

    uint32_t chunkIndex = gid.y;
    uint32_t meshletIndex = gid.x;

    uint32_t chunkID = payload.chunkIDs[chunkIndex];
    uint32_t lodLevel = payload.LODLevels[chunkIndex];
    uint32_t transitionMask = payload.transitionMasks[chunkIndex];

    if(chunkID >= voxelTerrainInfo.maxChunkCount) {
        isVisible = false;
    }

    float32_t3 chunkOrigin = float32_t3(GetChunkPos(chunkID));

    float32_t voxelSize = pow(1.0f, lodLevel);
    float32_t meshletDimSize = voxelSize * 4.0;

    uint32_t3 meshletCounts = uint32_t3(
        voxelTerrainInfo.chunkSize.x / meshletDimSize,
        voxelTerrainInfo.chunkSize.y / meshletDimSize,
        voxelTerrainInfo.chunkSize.z / meshletDimSize
    );

    float32_t3 meshletCoord = float32_t3(
        meshletIndex % meshletCounts.x,
        (meshletIndex / meshletCounts.x) % meshletCounts.y,
        meshletIndex / (meshletCounts.x * meshletCounts.y)
    );

    float32_t3 meshletOffset = meshletCoord * meshletDimSize;

    float32_t3 voxelCoord = float32_t3(
        gtid % 4,
        (gtid / 4) % 4,
        gtid / 16
    );

    float32_t3 voxelOffset = voxelCoord * voxelSize;
    float32_t3 basePos = chunkOrigin + meshletOffset + voxelOffset;

 
    /// -----------------------------------------------------------------------------
    /// 生成するメッシュのサイズを計算する
    /// -----------------------------------------------------------------------------


    uint32_t caseCode = 0;
    
    [unroll]
    for(uint32_t i=0; i<8; ++i) {
        float32_t3 samplePos = basePos - chunkOrigin + (kCornerOffsets[i] * voxelSize);

        /// チャンク内のローカルに合わせる
        float32_t density = GetDensity(samplePos, chunkID);

        if(density < 0.5f) {
            caseCode |= (1u << i);
        }
    }


    /// caseCodeに基づいてメッシュが生成可能かチェック
    uint32_t triangleCount = 0;
    uint32_t myVertOffset = 0;
    uint32_t myPrimOffset = 0;
    
    if(isVisible) {
        if(caseCode != 0 && caseCode != 255) {
            triangleCount = 1;
            InterlockedAdd(sVertexCount, 3, myVertOffset);
            InterlockedAdd(sPrimitiveCount, 1, myPrimOffset);

            sVertexOffsets[gtid] = myVertOffset;
            sPrimitiveOffsets[gtid] = myPrimOffset;
        }
    }
    

    GroupMemoryBarrierWithGroupSync();
    SetMeshOutputCounts(sVertexCount, sPrimitiveCount);
    GroupMemoryBarrierWithGroupSync();


    if(triangleCount > 0) {
        uint32_t vIdx = sVertexOffsets[gtid];
        uint32_t pIdx = sPrimitiveOffsets[gtid];

        float32_t3 centerPos = basePos;
        float32_t size = 1.0f;

        float32_t3 p0 = centerPos + float32_t3(-size, 0,    0);
        float32_t3 p1 = centerPos + float32_t3(    0, 0, size);
        float32_t3 p2 = centerPos + float32_t3( size, 0,    0);

        float32_t4 color = colors[chunkID % 8];
        if(chunkID == 0) {
            color = float32_t4(1,1,1,1);
        }

        verts[vIdx + 0].position = mul(float32_t4(p0, 1), viewProjection.matVP);
        verts[vIdx + 0].worldPosition = float4(p0, 1);
        verts[vIdx + 0].normal = float3(0, 1, 0);
        verts[vIdx + 0].color = color;

        verts[vIdx + 1].position = mul(float32_t4(p1, 1), viewProjection.matVP);
        verts[vIdx + 1].worldPosition = float4(p1, 1);
        verts[vIdx + 1].normal = float3(0, 1, 0);
        verts[vIdx + 1].color = color;

        verts[vIdx + 2].position = mul(float32_t4(p2, 1), viewProjection.matVP);
        verts[vIdx + 2].worldPosition = float4(p2, 1);
        verts[vIdx + 2].normal = float3(0, 1, 0);
        verts[vIdx + 2].color = color;

        tris[pIdx] = uint32_t3(vIdx + 0, vIdx + 1, vIdx + 2);
    }

}