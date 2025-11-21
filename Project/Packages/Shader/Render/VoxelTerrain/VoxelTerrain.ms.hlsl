#include "VoxelTerrain.hlsli"

static const uint3 kNumthreads = uint3(4, 4, 4);

struct VoxelVertexColor {
	float4 color[8];
};

//struct SubChunkVoxel {
//	float4 color[64]; /// 4*4*4(kNumthreads)の値
//};

/// 3x3x3のボクセルカラー情報 (中心[1][1][1]の周囲8頂点を含む)
struct VoxelColors {
	float4 color[3][3][3];
};

struct Vertices {
	VertexOut verts[4];
	//uint3 indis;
};

/// ---------------------------------------------------
/// Buffers
/// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

static const uint3 kTextureSize = uint3(100, 256, 100);
static const float3 kUVWOffset = float3(1, -1, 1);


/// ---------------------------------------------------
/// function
/// ---------------------------------------------------

/// 指定したボクセル位置の周囲8頂点の色を取得
VoxelVertexColor GetVoxelVertexColor(uint3 _voxelPos, uint _chunkTextureId) {
	float3 uvw = (float3(_voxelPos.xyz) + kUVWOffset) / float3(kTextureSize);
	uvw.y = 1.0f - uvw.y; // Y軸の反転
	VoxelVertexColor vvc;

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
	*/

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
	return vvc;
}

/// 指定したボクセル位置の頂点情報を取得
Vertices GetVoxelVertices(float3 _voxelPos, float4 _color) {
	Vertices verts;

	/// 上のコメントを参照して順番に頂点情報を設定、透明度から頂点の座標を調整
	/// 0: 手前左 ()
	float3 frontLeft = _voxelPos + float3(-0.5f, 0.0f, -0.5f);
	verts.verts[0].worldPosition = float4(frontLeft, 1);
	verts.verts[0].position = mul(float4(frontLeft, 1), viewProjection.matVP);
	
	
	/// 1: 手前右
	float3 frontRight = _voxelPos + float3(0.5f, 0.0f, -0.5f);
	verts.verts[1].worldPosition = float4(frontRight, 1);
	verts.verts[1].position = mul(float4(frontRight, 1), viewProjection.matVP);
	
	/// 2: 奥左
	float3 backLeft = _voxelPos + float3(-0.5f, 0.0f, 0.5f);
	verts.verts[2].worldPosition = float4(backLeft, 1.0f);
	verts.verts[2].position = mul(float4(backLeft, 1), viewProjection.matVP);
	
	/// 3: 奥右
	float3 backRight = _voxelPos + float3(0.5f, 0.0f, 0.5f);
	verts.verts[3].worldPosition = float4(backRight, 1.0f);
	verts.verts[3].position = mul(float4(backRight, 1), viewProjection.matVP);
	
	
	for (int i = 0; i < 4; i++) {
		verts.verts[i].normal = float3(0, 1, 0);
		verts.verts[i].color = _color;
	}

	return verts;
}


/// ---------------------------------------------------
/// Main
/// ---------------------------------------------------
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(4, 4, 4)]
void main(
    uint3 DTid : SV_DispatchThreadID,
	uint gi : SV_GroupIndex,
	uint3 groupId : SV_GroupID,
    in payload Payload asPayload,
    out vertices VertexOut verts[256],
    out indices uint3 indices[256]) {
	
	uint maxDrawVoxels = kNumthreads.x * kNumthreads.y * kNumthreads.z;
	uint numVertices = maxDrawVoxels * 4;
	uint numPrimitives = numVertices / 2;

	
	
	/// ボクセルの位置を計算、ボクセルは1x1x1なので中心に来るよう0.5を足す
	float3 chunkLocalPos = float3(DTid.xyz);
	/// チャンクの位置とボクセルのローカル位置からワールド座標を計算
	float3 chunkOrigin = asPayload.chunkOrigin;
	float3 worldPos = chunkLocalPos + asPayload.chunkOrigin;

	//AABB aabb;
	//aabb.min = worldPos;
	//aabb.max = worldPos + float3(asPayload.subChunkSize);
	//if(!IsVisible(aabb, CreateFrustumFromMatrix(viewProjection.matVP))) {
	//	/// カリングされた場合は頂点数・プリミティブ数を0にして終了
	//	numVertices = 0;
	//	numPrimitives = 0;
	//}


	/// とりあえずは最大値を設定
	SetMeshOutputCounts(numVertices, numPrimitives);
	if (numVertices == 0 || numPrimitives == 0) {
		return;
	}



	/// 3dTextureのuvw座標を計算
	float3 uvw = (float3(DTid.xyz) + kUVWOffset) / float3(kTextureSize);
	uvw.y = 1.0f - uvw.y; // Y軸の反転
	
	uint chunkTextureId = chunks[asPayload.chunkIndex].textureId;
	float4 voxelColor = voxelChunkTextures[chunkTextureId].SampleLevel(texSampler, uvw, 0);

	VoxelVertexColor vvc = GetVoxelVertexColor(DTid.xyz, chunkTextureId);
	float averageAlpha = 0.0f;
	for (int i = 0; i < 8; i++) {
		averageAlpha += vvc.color[i].a;
	}
	averageAlpha /= 8.0f;
	if (averageAlpha >= 1.0f) {
		voxelColor.rgb = 0.0f;
	} else if (averageAlpha != 0.0f) {
		voxelColor.rgb = 1.0f;
	}
	

	float offset = 0.5f;
	uint vIndex = gi * 4;
	if (vIndex + 3 < 256) {
		Vertices vs = GetVoxelVertices(worldPos, voxelColor);

		for (int i = 0; i < 4; i++) {
			verts[vIndex + i] = vs.verts[i];
		}
	}
	

	uint iIndex = gi * 2;
	if (iIndex < 256) {
		uint baseVIndex = iIndex / 2 * 4;
		indices[iIndex + 0] = uint3(baseVIndex + 0, baseVIndex + 2, baseVIndex + 1);
		indices[iIndex + 1] = uint3(baseVIndex + 2, baseVIndex + 3, baseVIndex + 1);
	}
}


/*
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(4, 4, 4)]
void main(
    uint3 DTid : SV_DispatchThreadID,
	uint gi : SV_GroupIndex,
	uint3 groupId : SV_GroupID,
    in payload Payload asPayload,
    out vertices VertexOut verts[256],
    out indices uint3 indices[256]) {
	
	uint maxDrawVoxels = kNumthreads.x * kNumthreads.y * kNumthreads.z;
	uint numVertices = maxDrawVoxels * 3;
	uint numPrimitives = numVertices / 3;


	/// とりあえずは最大値を設定
	SetMeshOutputCounts(numVertices, numPrimitives);
	if (numVertices == 0 || numPrimitives == 0) {
		return;
	}

	/// ボクセルの位置を計算
	int4 voxelPos = int4(DTid.xyz, 1);
	/// チャンクの位置とボクセルのローカル位置からワールド座標を計算
	float3 chunkOrigin = asPayload.chunkOrigin;
	float3 chunkLocalPos = float3(voxelPos.x, voxelPos.y, voxelPos.z);
	float3 worldPos = chunkLocalPos + asPayload.chunkOrigin;


	/// 3dTextureのuvw座標を計算
	float3 uvw = (float3(DTid.xyz) + float3(1, 0, 1)) / float3(kTextureSize);
	uvw.y = 1.0f - uvw.y; // Y軸の反転
	
	uint chunkTextureId = chunks[asPayload.chunkIndex].textureId;
	float4 voxelColor = voxelChunkTextures[chunkTextureId].SampleLevel(texSampler, uvw, 0);
	//voxelColor.a = 1.0f;


	float offset = 0.5f;
	
	uint vIndex = gi * 3;
	if (vIndex + 2 < 256) {
		verts[vIndex + 0].position = mul(float4(worldPos + float3(0, 0, offset), 1), viewProjection.matVP);
		verts[vIndex + 1].position = mul(float4(worldPos + float3(offset, 0, -offset), 1), viewProjection.matVP);
		verts[vIndex + 2].position = mul(float4(worldPos + float3(-offset, 0, -offset), 1), viewProjection.matVP);
	
		verts[vIndex + 0].worldPosition = float4(worldPos + float3(0, 0, offset), 1);
		verts[vIndex + 1].worldPosition = float4(worldPos + float3(offset, 0, -offset), 1);
		verts[vIndex + 2].worldPosition = float4(worldPos + float3(-offset, 0, -offset), 1);

		for (int i = 0; i < 3; i++) {
			verts[vIndex + i].normal = float3(0, 1, 0);
			verts[vIndex + i].color = voxelColor;
		}
	}
	
	
	uint iIndex = gi;
	if (iIndex < 256) {
		indices[iIndex] = uint3(vIndex + 0, vIndex + 1, vIndex + 2);
	}
	
}

*/