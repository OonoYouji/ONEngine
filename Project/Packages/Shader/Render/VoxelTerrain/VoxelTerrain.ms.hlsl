#include "VoxelTerrain.hlsli"

static const uint3 kNumthreads = uint3(4, 4, 4);

struct VoxelVertexColor {
	float4 color[9];
};

struct Vertices {
	VertexOut verts[8];
};

struct Indices {
	uint3 indis[12];
};

/// ---------------------------------------------------
/// Buffers
/// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

static const uint3 kTextureSize = uint3(100, 256, 100);
static const float3 kUVWOffset = float3(0.5f, -0.0f, 0.5f);


/// ---------------------------------------------------
/// function
/// ---------------------------------------------------


Indices GetIndices(uint _offset) {
	Indices indis;
	
	indis.indis[0] = uint3(0, 2, 1);
	indis.indis[1] = uint3(2, 3, 1);
	
	indis.indis[2] = uint3(2, 6, 3);
	indis.indis[3] = uint3(6, 7, 3);
	
	indis.indis[4] = uint3(6, 4, 5);
	indis.indis[5] = uint3(7, 6, 5);
	
	indis.indis[6] = uint3(4, 0, 1);
	indis.indis[7] = uint3(1, 4, 5);

	indis.indis[8] = uint3(1, 3, 5);
	indis.indis[9] = uint3(3, 7, 5);
	
	indis.indis[10] = uint3(4, 6, 2);
	indis.indis[11] = uint3(4, 2, 0);
	
	for (int i = 0; i < 12; i++) {
		indis.indis[i] += _offset;
	}

	return indis;
}


/// 指定したボクセル位置の周囲8頂点の色を取得
VoxelVertexColor GetVoxelVertexColor(uint3 _voxelPos, uint _chunkTextureId) {
	float3 uvw = (float3(_voxelPos.xyz) + kUVWOffset) / float3(kTextureSize);
	uvw.y = 1.0f - uvw.y; // Y軸の反転

	/*
		for (int i = 0; i <= 1; i++) のループで回したときの順番
		
		i = 0: (-0.5, -0.5, -0.5) -> 手前左下
		i = 1: ( 0.5, -0.5, -0.5) -> 手前右下
		i = 2: (-0.5,  0.5, -0.5) -> 手前左上
		i = 3: ( 0.5,  0.5, -0.5) -> 手前右上
		i = 4: (-0.5, -0.5,  0.5) -> 奥左下
		i = 5: ( 0.5, -0.5,  0.5) -> 奥右下
		i = 6: (-0.5,  0.5,  0.5) -> 奥左上
		i = 7: ( 0.5,  0.5,  0.5) -> 奥右上
		i = 8: ( 0.0,  0.0,  0.0) -> 中心
	*/

	VoxelVertexColor vvc;
	[unroll]
	for (int i = 0; i < 8; i++) {
		float3 offset = float3(
			(i & 1) ? 0.5f : -0.5f,
			(i & 2) ? 0.5f : -0.5f,
			(i & 4) ? 0.5f : -0.5f
		);
		float3 sampleUVW = uvw + offset / float3(kTextureSize);
		vvc.color[i] = voxelChunkTextures[_chunkTextureId].SampleLevel(texSampler, sampleUVW, 0);
	}

	/// 中心の色も取得
	vvc.color[8] = voxelChunkTextures[_chunkTextureId].SampleLevel(texSampler, uvw, 0);

	return vvc;
}

/// 呼び出し元ではすでに描画の可否は判定済みなので、ここでは単純に頂点情報を返すだけ
Vertices GetVoxelVertices(VoxelVertexColor _voxelColors, float3 _voxelPos, float4 _color) {
	Vertices verts;
	
	[unroll]
	for (int i = 0; i < 8; i++) {
		float3 offset = float3(
			(i & 1) ? 0.45f : -0.45f,
			(i & 2) ? 0.45f : -0.45f,
			(i & 4) ? 0.45f : -0.45f
		);
		
		float3 vertexPos = offset + _voxelPos;
		verts.verts[i].worldPosition = float4(vertexPos, 1);

		float heightFactor = vertexPos.y / kTextureSize.y;
		verts.verts[i].color = float4(1 - heightFactor, 1, 1 - heightFactor, 1);
		verts.verts[i].position = mul(verts.verts[i].worldPosition, viewProjection.matVP);
	}

	Indices indis = GetIndices(0);
	for (int i = 0; i < 12; i++) {

		VertexOut v0 = verts.verts[indis.indis[i].x];
		v0.otherVertexPos1 = verts.verts[indis.indis[i].y].worldPosition;
		v0.otherVertexPos2 = verts.verts[indis.indis[i].z].worldPosition;

		verts.verts[indis.indis[i].x] = v0;
	}
	
	return verts;
}




/// ---------------------------------------------------
/// Main
/// ---------------------------------------------------
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(2, 2, 2)]
void main(
    uint3 DTid : SV_DispatchThreadID,
	uint gi : SV_GroupIndex,
	uint3 groupId : SV_GroupID,
    in payload Payload asPayload,
    out vertices VertexOut verts[256],
    out indices uint3 indices[256]) {
	
	/// グループ内の最初のスレッドのみ処理
	if (gi != 0) {
		return;
	}
	
	
	/// forループですべてのボクセルを処理、最適な頂点数、プリミティブ数しか出力しない

	uint drawVoxelCount = 0;
	uint3 drawVoxelPositions[64];
	VoxelVertexColor drawVoxelVertexColors[64];
	
	/// ---------------------------------------------------
	/// 事前にカリング、ボクセルごとに描画するか判定
	/// ---------------------------------------------------

	AABB aabb;
	aabb.min = asPayload.chunkOrigin + DTid;
	aabb.max = aabb.min + asPayload.subChunkSize;
	if (IsVisible(aabb, CreateFrustumFromMatrix(viewProjection.matVP))) {

		[unroll]
		for (int z = 0; z < 2; z++) {
			[unroll]
			for (int y = 0; y < 4; y++) {
				[unroll]
				for (int x = 0; x < 2; x++) {
					uint3 voxelPos = uint3(x, y, z) + DTid;
				
					/// voxelPosを中心に3x3x3のボクセルカラー情報を取得
					uint chunkTextureId = chunks[asPayload.chunkIndex].textureId;
					VoxelVertexColor vvc = GetVoxelVertexColor(voxelPos, chunkTextureId);
					if (vvc.color[8].a == 0.0f) {
						/// 中心のボクセルが透明なら描画しない
						continue;
					}
					
					float averageAlpha = 0.0f;
					for (int i = 0; i < 8; i++) {
						averageAlpha += vvc.color[i].a;
					}
					averageAlpha /= 8.0f;
					if (averageAlpha >= 1.0f) {
						/// 周囲のボクセルがすべて不透明なら描画しない
					} else if (averageAlpha != 0.0f) {
						/// 周囲に透明なボクセルがある場合は描画する
						drawVoxelCount++;
						drawVoxelPositions[drawVoxelCount - 1] = voxelPos;
						drawVoxelVertexColors[drawVoxelCount - 1] = vvc;

					}

				}
			}
		}
	}

	
	/// 描画するボクセル数に応じて頂点数、プリミティブ数を計算
	uint numVertices = drawVoxelCount * 8;
	uint numPrimitives = drawVoxelCount * 12;

	SetMeshOutputCounts(numVertices, numPrimitives);
	if (numVertices == 0 || numPrimitives == 0) {
		return;
	}


	/// ---------------------------------------------------
	/// ボクセルごとに頂点、インデックスを設定
	/// ---------------------------------------------------

	for (uint i = 0; i < drawVoxelCount; i++) {
		uint3 voxelPos = drawVoxelPositions[i];
		
		uint vIndex = i * 8;
		if (vIndex + 3 < numVertices) {
			float3 worldPos = float3(voxelPos) + asPayload.chunkOrigin;
			
			Vertices vs = GetVoxelVertices(drawVoxelVertexColors[i], worldPos, float4(1, 0, 0, 1));
			for (int j = 0; j < 8; j++) {
				verts[vIndex + j] = vs.verts[j];
			}
		}

		uint iIndex = i * 12;
		if (iIndex + 11 < numPrimitives) {
			uint baseVIndex = i * 8;
			Indices indis = GetIndices(baseVIndex);
			
			//[uroll]
			for (int j = 0; j < 12; ++j) {
				indices[iIndex + j] = indis.indis[j];
			}
		}
		
	}

}
