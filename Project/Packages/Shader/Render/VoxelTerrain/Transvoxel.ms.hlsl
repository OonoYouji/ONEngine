#include "Transvoxel.hlsli"
#include "TransvoxelTables.hlsli"

// -----------------------------------------------------------------------------
// Resources
// -----------------------------------------------------------------------------
Texture3D<float> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------
static const uint kTransitionCellStride = 10;

// Transvoxel 13 nodes (face + interior)
static const float3 kTransitionNodes[13] =
{
    float3(0,0,0), float3(1,0,0), float3(0,1,0), float3(1,1,0), // 0-3
    float3(0.5,0,0), float3(0,0.5,0), float3(0.5,0.5,0),
    float3(1,0.5,0), float3(0.5,1,0),                           // 4-8
    float3(0,0,1), float3(1,0,1), float3(0,1,1), float3(1,1,1)  // 9-12
};

// -----------------------------------------------------------------------------
// Transvoxel Table Helpers
// -----------------------------------------------------------------------------
uint GetTransitionClassId(uint caseCode)
{
    uint index = caseCode >> 2;
    uint shift = (caseCode & 3) << 3;
    return (index < 128) ? ((tTransitionCellClass[index] >> shift) & 0xFF) : 0;
}

uint GetTransitionDataByte(uint classId, uint byteOffset)
{
    uint ptr   = classId * kTransitionCellStride + (byteOffset >> 2);
    uint shift = (byteOffset & 3) << 3;
    return (tTransitionCellData[ptr] >> shift) & 0xFF;
}

// -----------------------------------------------------------------------------
// Density Sampling (Texture3D 専用・安全版)
// -----------------------------------------------------------------------------
float GetDensity(float3 localPos, uint _chunkId)
{
    float3 texSize = float3(voxelTerrainInfo.textureSize);
    float3 uvw = localPos / texSize;

    uvw.y = 1.0f - uvw.y;

	uint chunkId = _chunkId;
    
    // チャンクのグリッド座標を計算
	int chunkX = int(chunkId) % int(voxelTerrainInfo.chunkCountXZ.x);
	int chunkZ = int(chunkId) / int(voxelTerrainInfo.chunkCountXZ.x);
    
    /// uvwが0-1範囲外であれば隣のチャンクを参照する
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
    

    return voxelChunkTextures[chunks[chunkId].textureId].SampleLevel(texSampler, uvw, 0);
}

// -----------------------------------------------------------------------------
// Mesh Shader
// -----------------------------------------------------------------------------
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
    uint3 DTid : SV_DispatchThreadID,
    in payload Payload payload,
    out vertices VertexOut verts[16],
    out indices  uint3 tris[16]
)
{
    // -------------------------------------------------------------------------
    // 1. Face & Cell Decode
    // -------------------------------------------------------------------------
    uint res = payload.faceResolution;
    uint cellsPerFace = res * res;

    uint faceSeq = DTid.x / cellsPerFace;
    uint cellIdx = DTid.x % cellsPerFace;

    uint checkBits[4] = { 0, 1, 4, 5 }; // -X +X -Z +Z

    uint activeDir = 0xFFFFFFFF;
    uint counter = 0;

    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        if (payload.transitionMask & (1u << checkBits[i]))
        {
            if (counter == faceSeq)
            {
                activeDir = checkBits[i];
                break;
            }
            counter++;
        }
    }

    bool valid = (activeDir != 0xFFFFFFFF);

    uint numVerts = 0;
    uint numTris  = 0;
    uint classId  = 0;

    float densities[13];
    float3 uVec, vVec, wVec;
    float3 basePos;
    float step = float(payload.subChunkSize.x);

    // -------------------------------------------------------------------------
    // 2. Coordinate Setup
    // -------------------------------------------------------------------------
    if (valid)
    {
        uint u = cellIdx % res;
        uint v = cellIdx / res;
        uint m = res - 1;

        float3 offset;

        if (activeDir == 0)      { offset=float3(0,u,v)*step; uVec=float3(0,1,0); vVec=float3(0,0,1); wVec=float3(-1,0,0); }
        else if (activeDir == 1) { offset=float3(m,u,v)*step; uVec=float3(0,1,0); vVec=float3(0,0,1); wVec=float3( 1,0,0); }
        else if (activeDir == 4) { offset=float3(u,v,0)*step; uVec=float3(1,0,0); vVec=float3(0,1,0); wVec=float3(0,0,-1); }
        else                     { offset=float3(u,v,m)*step; uVec=float3(1,0,0); vVec=float3(0,1,0); wVec=float3(0,0, 1); }

        basePos = payload.chunkOrigin + offset;

        // ---------------------------------------------------------------------
        // 3. Density Sampling
        // ---------------------------------------------------------------------
        [unroll]
        for (uint i = 0; i < 13; ++i)
        {
            float3 local = kTransitionNodes[i];
            float3 pos   = offset + (local.x * uVec + local.y * vVec + local.z * wVec) * step;
            densities[i] = GetDensity(pos, payload.chunkIndex);
        }

        // 中間点の補正（Transvoxel 必須）
        densities[4] = (densities[0] + densities[1]) * 0.5f;
        densities[5] = (densities[0] + densities[2]) * 0.5f;
        densities[7] = (densities[1] + densities[3]) * 0.5f;
        densities[8] = (densities[2] + densities[3]) * 0.5f;
        densities[6] = (densities[0] + densities[1] + densities[2] + densities[3]) * 0.25f;

        // ---------------------------------------------------------------------
        // 4. Case Code
        // ---------------------------------------------------------------------
        uint caseCode = 0;
        [unroll]
        for (uint i = 0; i < 13; ++i)
            caseCode |= (densities[i] < kIsoLevel) ? (1u << i) : 0;

        if (caseCode != 0 && caseCode != 0x1FFF)
        {
            classId = GetTransitionClassId(caseCode);
            if ((classId & 0x7F) != 0)
            {
                uint header = GetTransitionDataByte(classId, 0);
                numVerts = (header >> 4) & 0x0F;
                numTris  = header & 0x0F;
            }
        }
    }

    // -------------------------------------------------------------------------
    // 5. Output Counts (全スレッド必須)
    // -------------------------------------------------------------------------
    SetMeshOutputCounts(numVerts, numTris);

    // -------------------------------------------------------------------------
    // 6. Write Geometry
    // -------------------------------------------------------------------------
    if (numVerts == 0)
        return;

    [unroll]
    for (uint i = 0; i < numVerts; ++i)
    {
        uint edge = GetTransitionDataByte(classId, 1 + i);
        uint a = edge >> 4;
        uint b = edge & 0x0F;

        float t = saturate((kIsoLevel - densities[a]) / (densities[b] - densities[a]));
        float3 local = lerp(kTransitionNodes[a], kTransitionNodes[b], t);

        float3 pos =
            basePos +
            (local.x * uVec + local.y * vVec + local.z * wVec) * step;

        verts[i].worldPosition = float4(pos, 1);
        verts[i].positionCS   = mul(float4(pos,1), viewProjection.matVP);
        verts[i].normal       = float3(0,1,0); // 仮
        verts[i].color        = float4(1,1,1,1);
    }

    uint triBase = 1 + numVerts;
    [unroll]
    for (uint i = 0; i < numTris; ++i)
    {
        tris[i] = uint3(
            GetTransitionDataByte(classId, triBase + i*3 + 0),
            GetTransitionDataByte(classId, triBase + i*3 + 1),
            GetTransitionDataByte(classId, triBase + i*3 + 2)
        );
    }
}
