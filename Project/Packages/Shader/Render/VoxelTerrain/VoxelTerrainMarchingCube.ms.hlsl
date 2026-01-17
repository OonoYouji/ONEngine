#include "VoxelTerrain.hlsli"
#include "../VoxelTerrainTest/Table.hlsli"

// マーチングキューブ用の定数定義
static const float kIsoLevel = 0.5f;

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
VertexOut VertexInterp(float3 p1, float3 p2, float3 _chunkOrigin,float3 subChunkSize, float d1, float d2, uint _chunkId) {
	VertexOut vOut;
	
	// 補間係数の計算（ゼロ除算対策）
	float denom = d2 - d1;
	float t = 0.5f; // デフォルト値
	
	if (abs(denom) > 0.00001f) {
		t = (kIsoLevel - d1) / denom;
		t = saturate(t); // [0,1]にクランプ
	}
	
	float3 localPos = lerp(p1, p2, t);
	
	vOut.worldPosition = float4(localPos + _chunkOrigin, 1.0f);
    vOut.worldPosition.xz += (subChunkSize / 2.0).xz; 

	vOut.position = mul(vOut.worldPosition, viewProjection.matVP);
	
	// 補間位置での法線を計算
	vOut.normal = GetGradient(localPos, _chunkId);
	
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


// ---------------------------------------------------
// Main Mesh Shader
// ---------------------------------------------------
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
	uint3 DTid : SV_DispatchThreadID,
	uint gi : SV_GroupIndex,
	in payload Payload asPayload,
	out vertices VertexOut verts[16],
	out indices uint3 indis[6]) {

	uint3 step = asPayload.subChunkSize;
	float3 basePos = float3(DTid * step);

	float cubeDensities[8];
	uint cubeIndex = 0;
	
	[unroll]
	for (int i = 0; i < 8; ++i) {
		float3 samplePos = basePos + (kCornerOffsets[i] * float3(step));

		float d = GetDensity(samplePos, asPayload.chunkIndex);
		cubeDensities[i] = d;
		
		if (d < kIsoLevel) {
			cubeIndex |= (1u << i);
		}
	}

	uint triCount = 0;
	[unroll]
	for (int i = 0; i < 15; i += 3) {
		triCount += (TriTable[cubeIndex][i] != -1) ? 1 : 0;
	}
	
	SetMeshOutputCounts(triCount * 3, triCount);
	
	for (uint t = 0; t < triCount; t++) {

		VertexOut otuVerts[3];

		for (int v = 0; v < 3; v++) {
			int edgeIndex = TriTable[cubeIndex][(t * 3) + v];
			
			int idx1 = kEdgeConnection[edgeIndex].x;
			int idx2 = kEdgeConnection[edgeIndex].y;
			
			float3 p1 = basePos + (kCornerOffsets[idx1] * float3(step));
			float3 p2 = basePos + (kCornerOffsets[idx2] * float3(step));
			
			otuVerts[v] = VertexInterp(p1, p2, asPayload.chunkOrigin, float32_t3(asPayload.subChunkSize), cubeDensities[idx1], cubeDensities[idx2], asPayload.chunkIndex);
			verts[(t * 3) + v] = otuVerts[v];
		}
		
		float3 normal = GetNormal(
			otuVerts[0].worldPosition.xyz,
			otuVerts[1].worldPosition.xyz,
			otuVerts[2].worldPosition.xyz
		);
		
		verts[(t * 3) + 0].normal = normal;
		verts[(t * 3) + 1].normal = normal;
		verts[(t * 3) + 2].normal = normal;
		
		indis[t] = uint3(
			(t * 3) + 0,
			(t * 3) + 1,
			(t * 3) + 2
		);
	}
}