#include "BrushPreview.hlsli"
#include "../VoxelTerrainTest/Table.hlsli"

Texture3D<float4> voxelChunkTextures[] : register(t2);
SamplerState texSampler : register(s0);


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


// 勾配(法線)取得の修正版
float3 GetGradient(float3 _localPos, uint _chunkId) {
	// ボクセルサイズに合わせたステップ幅
	// stepを小さくしすぎると数値誤差が大きくなるので注意
	float step = 0.5f;
	
	// 中心差分法で勾配を計算
	float dx = GetDensity(_localPos + float3(step, 0, 0), _chunkId) - GetDensity(_localPos - float3(step, 0, 0), _chunkId);
	float dy = GetDensity(_localPos + float3(0, step, 0), _chunkId) - GetDensity(_localPos - float3(0, step, 0), _chunkId);
	float dz = GetDensity(_localPos + float3(0, 0, step), _chunkId) - GetDensity(_localPos - float3(0, 0, step), _chunkId);

	float3 grad = float3(dx, dy, dz);
	float len = length(grad);
	
	// 勾配がほぼゼロの場合（平坦な領域）
	if (len < 0.00001f) {
		return float3(0, 1, 0); // デフォルトで上向き
	}
	
	// 正規化して返す（符号に注意）
	return -normalize(grad);
}

// 頂点補間
VertexOut VertexInterp(float3 p1, float3 p2, float3 chunkOrigin,float3 subChunkSize, float d1, float d2, uint chunkId) {
	VertexOut vOut;
	
	// 補間係数の計算（ゼロ除算対策）
	float denom = d2 - d1;
	float t = 0.5f; // デフォルト値
	
	if (abs(denom) > 0.00001f) {
		t = (voxelTerrainInfo.isoLevel - d1) / denom;
		t = saturate(t); // [0,1]にクランプ
	}
	
	float3 localPos = lerp(p1, p2, t);
	
	vOut.worldPosition = float4(localPos + chunkOrigin, 1.0f);
    vOut.worldPosition.xz += (subChunkSize / 2.0).xz; 

	vOut.position = mul(vOut.worldPosition, viewProjection.matVP);
	
	return vOut;
}


float3 GetNormal(float3 p0, float3 p1, float3 p2) {
	float3 u = p1 - p0;
	float3 v = p2 - p0;
	return normalize(cross(u, v));
}

float32_t3 GetBasePos(uint32_t id, uint32_t3 size, uint32_t3 step) {
    // uint32_t3 size = voxelTerrainInfo.chunkSize;
    uint32_t3 gridPos = uint32_t3(
        id % size.x,
        (id / size.x) % size.y,
        id / (size.x * size.y)
    );

    return float32_t3(gridPos * step);
}


bool CheckInside(float3 mousePoint, uint32_t radius, float3 aabbMin, float3 aabbMax) {
    float3 closestPoint = float3(
        clamp(mousePoint.x, aabbMin.x, aabbMax.x),
        clamp(mousePoint.y, aabbMin.y, aabbMax.y),
        clamp(mousePoint.z, aabbMin.z, aabbMax.z)
    );

    float distanceSq = dot(closestPoint - mousePoint, closestPoint - mousePoint);
    return distanceSq <= (radius * radius);
}

bool CheckInside(float3 mousePoint, uint32_t radius, float3 samplePos) {
    float3 diff = samplePos - mousePoint;
    float distanceSq = dot(diff, diff);
    return distanceSq <= (radius * radius);
}

[shader("mesh")]
[outputtopology("triangle")]
[numthreads(16, 1, 1)]
void main(
	uint3 DTid : SV_DispatchThreadID,
	in payload Payload asPayload,
	out vertices VertexOut verts[240],
	out indices uint3 indis[80]) {

	uint3 step = asPayload.subChunkSize;
	float3 basePos = GetBasePos(DTid.x, asPayload.chunkSize, step);
    
    float3 worldBasePos = basePos + asPayload.chunkOrigin;

    uint32_t3 chunkSize = uint32_t3(voxelTerrainInfo.chunkSize);
    uint32_t transitionCode = 0;
    
	float cubeDensities[8];
	uint cubeIndex = 0;
	uint triCount = 0;
	
    bool isInside = false;
	[unroll]
	for (int i = 0; i < 8; ++i) {
		float3 samplePos = basePos + (kCornerOffsets[i] * float3(step));
        
		float d = GetDensity(samplePos, asPayload.chunkIndex);
        if(CheckInside(
            asPayload.brushWorldPos, BrushInfo.brushRadius, 
            samplePos + asPayload.chunkOrigin)) {
            d += BrushInfo.brushStrength;
            isInside = true;
        }
        
		cubeDensities[i] = d;
		if (d < voxelTerrainInfo.isoLevel) {
			cubeIndex |= (1u << i);
		}
	}
    
    if(isInside) {
	    [unroll]
	    for (int i = 0; i < 5; i++) {
	    	triCount += (TriTable[cubeIndex][i * 3] != -1) ? 1 : 0;
	    }
    }

    uint outputTriOffset = WavePrefixSum(triCount);
    uint totalTriCount = WaveActiveSum(triCount);

    // GroupMemoryBarrier();
    SetMeshOutputCounts(totalTriCount * 3, totalTriCount);
    if(triCount == 0) {
        return;
    }

	
	for (uint t = 0; t < triCount; t++) {
        uint currentTriIndex = outputTriOffset + t;
        uint vIndex = currentTriIndex * 3;
        uint pIndex = currentTriIndex;

		VertexOut outVerts[3];

		for (int v = 0; v < 3; v++) {
			int edgeIndex = TriTable[cubeIndex][(t * 3) + v];
			
			int idx1 = kEdgeConnection[edgeIndex].x;
			int idx2 = kEdgeConnection[edgeIndex].y;
			
			float3 p1 = basePos + (kCornerOffsets[idx1] * float3(step));
			float3 p2 = basePos + (kCornerOffsets[idx2] * float3(step));
			
			outVerts[v] = VertexInterp(p1, p2, asPayload.chunkOrigin, float32_t3(asPayload.subChunkSize), cubeDensities[idx1], cubeDensities[idx2], asPayload.chunkIndex);
			verts[vIndex + v] = outVerts[v];
		}
		
		float3 normal = GetNormal(
			outVerts[0].worldPosition.xyz,
			outVerts[1].worldPosition.xyz,
			outVerts[2].worldPosition.xyz
		);
		
		verts[vIndex + 0].normal = normal;
		verts[vIndex + 1].normal = normal;
		verts[vIndex + 2].normal = normal;
		
		indis[pIndex] = uint3(
			vIndex + 0,
			vIndex + 1,
			vIndex + 2
		);
	}
}