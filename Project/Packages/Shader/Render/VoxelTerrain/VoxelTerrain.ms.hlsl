#include "VoxelTerrain.hlsli"

/// ---------------------------------------------------
/// Buffers
/// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);

static const uint3 kTextureSize = uint3(100, 256, 100);

static const uint3 kNumthreads = uint3(4, 4, 4);
static const float4 kColors[6] = {
	float4(1, 0, 0, 1),
	float4(0, 1, 0, 1),
	float4(0, 0, 1, 1),
	float4(1, 1, 0, 1),
	float4(1, 0, 1, 1),
	float4(0, 1, 1, 1)
};

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
	uint numVertices = maxDrawVoxels * 3;
	uint numPrimitives = numVertices / 3;


	/// とりあえずは最大値を設定
	SetMeshOutputCounts(numVertices, numPrimitives);
	if (numVertices == 0 || numPrimitives == 0) {
		return;
	}

	/// 3dTextureのuvw座標を計算
	float3 uvw = float3(DTid.xyz) / float3(kNumthreads);

	
	/// ボクセルの位置を計算
	int4 voxelPos = int4(DTid.xyz, 1);
	
	uint chunkTextureId = chunks[asPayload.chunkIndex].textureId;
	float4 voxelColor = voxelChunkTextures[chunkTextureId].SampleLevel(texSampler, uvw, 0);
	voxelColor.a = 1.0f;
	
	/// チャンクの位置とボクセルのローカル位置からワールド座標を計算
	float3 chunkOrigin = asPayload.chunkOrigin;
	float3 chunkLocalPos = float3(voxelPos.x, voxelPos.y, voxelPos.z);
	float3 worldPos = chunkLocalPos + asPayload.chunkOrigin;

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

	/// 3dTextureのuvw座標を計算
	float3 uvw = float3(DTid.xyz) / float3(kNumthreads);

	/// ボクセルの位置を計算
	int4 loadPos = int4(DTid.xyz, 1);
	uint chunkTextureId = chunks[asPayload.chunkIndex].textureId;
	float4 voxelColor = voxelChunkTextures[chunkTextureId].Load(loadPos);

	/// Dispatchごとに色を決める
	uint3 dispatchDim = asPayload.dispatchSize;
	uint dispatchIndex = IndexOfMeshGroup(groupId, dispatchDim);
	float4 color = kColors[dispatchIndex % 6];

	/// チャンクの位置とボクセルのローカル位置からワールド座標を計算
	float3 chunkOrigin = asPayload.chunkOrigin;
	float3 chunkLocalPos = float3(loadPos.x, loadPos.y, loadPos.z);
	float3 worldPos = chunkLocalPos + asPayload.chunkOrigin;

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
			verts[vIndex + i].color = color;
		}
	}
	
	
	uint iIndex = gi;
	if (iIndex < 256) {
		indices[iIndex] = uint3(vIndex + 0, vIndex + 1, vIndex + 2);
	}
	
}
*/