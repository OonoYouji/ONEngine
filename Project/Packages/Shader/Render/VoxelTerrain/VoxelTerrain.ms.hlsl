#include "VoxelTerrain.hlsli"

/// ---------------------------------------------------
/// Buffers
/// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);


static const uint3 kNumthreads = uint3(8, 8, 2);

/// ---------------------------------------------------
/// Main
/// ---------------------------------------------------
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(8, 8, 2)]
void main(
    uint3 DTid : SV_DispatchThreadID,
	uint gi : SV_GroupIndex,
    in payload Payload asPayload,
    out vertices VertexOut verts[256],
    out indices uint3 indices[256]) {

	/// とりあえずは最大値を設定
	SetMeshOutputCounts(256, 256 / 3);

	/// 3dTextureのuvw座標を計算
	float3 uvw = float3(DTid.xyz) / float3(kNumthreads);

	uint chunkTextureId = chunks[asPayload.chunkIndex].textureId;
	float4 voxelColor = voxelChunkTextures[chunkTextureId].SampleLevel(texSampler, uvw, 0);

	/// チャンクの位置とボクセルのローカル位置からワールド座標を計算
	float3 chunkOrigin = asPayload.chunkOrigin;
	float3 chunkLocalPos = uvw * voxelTerrainInfo.chunkSize;
	float3 worldPos = chunkOrigin + chunkLocalPos;
	
	uint vIndex = gi * 3;
	if (vIndex + 2 < 256) {
		verts[vIndex + 0].position = mul(float4(worldPos + float3(0, 0, 1), 1), viewProjection.matVP);
		verts[vIndex + 1].position = mul(float4(worldPos + float3(1, 0, -1), 1), viewProjection.matVP);
		verts[vIndex + 2].position = mul(float4(worldPos + float3(-1, 0, -1), 1), viewProjection.matVP);
	
		verts[vIndex + 0].worldPosition = float4(worldPos + float3(0, 0, 1), 1);
		verts[vIndex + 1].worldPosition = float4(worldPos + float3(1, 0, -1), 1);
		verts[vIndex + 2].worldPosition = float4(worldPos + float3(-1, 0, -1), 1);

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