#include "VoxelTerrain.hlsli"
#include "../VoxelTerrainTest/Table.hlsli"
#include "../../Texture.hlsli"

// ---------------------------------------------------
// Buffers
// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[kMaxTextureCount] : register(t1);
SamplerState texSampler : register(s0);

// ---------------------------------------------------
// Functions
// ---------------------------------------------------

float3 GetVoxelSize() {
	return float3(voxelTerrainInfo.chunkSize) / float3(voxelTerrainInfo.textureSize);
}

// 密度取得
float GetDensity(float3 worldPos) {
    uint32_t2 chunkLocalID = uint32_t2(worldPos.xz) / uint32_t2(voxelTerrainInfo.chunkSize.xz);

    // ローカル座標をUVW座標に変換（各軸で異なるサイズを使用）
    float32_t3 chunkOrigin = float32_t3(
        chunkLocalID.x * voxelTerrainInfo.chunkSize.x, 
        0,
        chunkLocalID.y * voxelTerrainInfo.chunkSize.z
    );

	float3 uvw = (worldPos - chunkOrigin) / float3(voxelTerrainInfo.textureSize);
	uvw.y = 1.0f - uvw.y;
    
    // Y方向の範囲外処理（空は空気、地下は固体）
    if (uvw.y <= 0.0f) { return 0.0f; } // 空
    if (uvw.y >= 1.0f) { return 1.0f; } // 地下    

	uint chunkId = chunkLocalID.x + chunkLocalID.y * uint(voxelTerrainInfo.chunkCountXZ.x);
	return voxelChunkTextures[chunks[chunkId].textureId].SampleLevel(texSampler, uvw, 0).a;
}


float32_t4 GetVolumeTextureColor(float32_t3 worldPos) {
    uint32_t2 chunkLocalID = uint32_t2(worldPos.xz) / uint32_t2(voxelTerrainInfo.chunkSize.xz);

    // ローカル座標をUVW座標に変換（各軸で異なるサイズを使用）
    float32_t3 chunkOrigin = float32_t3(
        chunkLocalID.x * voxelTerrainInfo.chunkSize.x, 
        0,
        chunkLocalID.y * voxelTerrainInfo.chunkSize.z
    );

	float3 uvw = (worldPos - chunkOrigin) / float3(voxelTerrainInfo.textureSize);
	uvw.y = 1.0f - uvw.y;
    
	uint chunkId = chunkLocalID.x + chunkLocalID.y * uint(voxelTerrainInfo.chunkCountXZ.x);
	return voxelChunkTextures[chunks[chunkId].textureId].SampleLevel(texSampler, uvw, 0);
}



float3 CalculateNormal(float3 pos, float step) {
    float eps = step; 

    float dx = GetDensity(pos + float3(eps, 0, 0)) - GetDensity(pos - float3(eps, 0, 0));
    float dy = GetDensity(pos + float3(0, eps, 0)) - GetDensity(pos - float3(0, eps, 0));
    float dz = GetDensity(pos + float3(0, 0, eps)) - GetDensity(pos - float3(0, 0, eps));

    float3 grad = float3(dx, dy, dz);
    float sqLen = dot(grad, grad);

    if (sqLen < 1.0e-10f) return float3(0, 1, 0);

    return normalize(-grad);
}

// 頂点補間
VertexOut VertexInterp(float3 p1, float3 p2, float3 subChunkSize, float d1, float d2) {
	VertexOut vOut;
	
	// 補間係数の計算（ゼロ除算対策）
	float denom = d2 - d1;
	float t = 0.5f; // デフォルト値
	
	if (abs(denom) > 0.00001f) {
		t = (voxelTerrainInfo.isoLevel - d1) / denom;
		t = saturate(t); // [0,1]にクランプ
	}
	
	float3 localPos = lerp(p1, p2, t);
	float3 voxelSize = GetVoxelSize();
	float3 worldPos = (localPos * voxelSize);
	worldPos.xz += (subChunkSize * voxelSize * 0.5f).xz;

	vOut.worldPosition = float4(worldPos, 1.0f);
	vOut.position      = mul(vOut.worldPosition, viewProjection.matVP);
    // vOut.normal        = CalculateNormal(worldPos, subChunkSize.x);

    float3 n1 = CalculateNormal(p1, subChunkSize.x);
    float3 n2 = CalculateNormal(p2, subChunkSize.x);
    vOut.normal = normalize(lerp(n1, n2, t));

    vOut.color = GetVolumeTextureColor(worldPos);
	
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


// ---------------------------------------------------
// マーチングキューブ法の1ボクセルが表示する最大頂点は 15頂点 なので
// 2*4*2= 16
// 16*15=240頂点, 16*5=80三角形
// ---------------------------------------------------
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(16, 1, 1)]
void main(
	uint3 DTid : SV_DispatchThreadID,
	in payload Payload asPayload,
	out vertices VertexOut verts[240],
	out indices uint3 indis[80]) {

	uint3 step = asPayload.subChunkSize;
	float32_t3 worldPos = GetBasePos(DTid.x, asPayload.chunkSize, step);
    worldPos += asPayload.startPos;

	float cubeDensities[8];
	uint cubeIndex = 0;
	uint triCount = 0;

	[unroll]
	for (int i = 0; i < 8; ++i) {
		float d = GetDensity(worldPos + (kCornerOffsets[i] * float3(step)));
		cubeDensities[i] = d;
    
		if (d < voxelTerrainInfo.isoLevel) {
			cubeIndex |= (1u << i);
		}
	}

	[unroll]
	for (int i = 0; i < 15; i += 3) {
		triCount += (TriTable[cubeIndex][i] != -1) ? 1 : 0;
	}

    uint outputTriOffset = WavePrefixSum(triCount);
    uint totalTriCount   = WaveActiveSum(triCount);

    SetMeshOutputCounts(totalTriCount * 3, totalTriCount);
	
	for (uint t = 0; t < triCount; t++) {
        uint currentTriIndex = outputTriOffset + t;
        uint vIndex = currentTriIndex * 3;
        uint pIndex = currentTriIndex;

		for (int v = 0; v < 3; v++) {
			int edgeIndex = TriTable[cubeIndex][(t * 3) + v];
			
			int idx1 = kEdgeConnection[edgeIndex].x;
			int idx2 = kEdgeConnection[edgeIndex].y;
			
			float3 p1 = worldPos + (kCornerOffsets[idx1] * float3(step));
			float3 p2 = worldPos + (kCornerOffsets[idx2] * float3(step));
			
			verts[vIndex + v] = VertexInterp(p1, p2, float32_t3(asPayload.subChunkSize), cubeDensities[idx1], cubeDensities[idx2]);
		}
		
		indis[pIndex] = uint3(
			vIndex + 0,
			vIndex + 1,
			vIndex + 2
		);
	}
}