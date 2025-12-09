#include "../Render/VoxelTerrain/VoxelTerrainCommon.hlsli"
#include "../ConstantBufferData/ViewProjection.hlsli"
#include "../Math/Math.hlsli"

struct InputInfo {
	float2 screenMousePos;
	uint mouseLeftButton;
	uint keyboardLShift;
};

struct EditorInfo {
	float brushRadius;
};


/// ///////////////////////////////////////////////////
/// buffers
/// ///////////////////////////////////////////////////

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection> viewProjection : register(b1);
ConstantBuffer<Camera> camera : register(b2);
ConstantBuffer<InputInfo> inputInfo : register(b3);
ConstantBuffer<EditorInfo> editorInfo : register(b4);

StructuredBuffer<Chunk> chunks : register(t0);
Texture2D<float4> worldPositionTexture : register(t1);
RWTexture3D<float4> voxelTextures[] : register(u0);
SamplerState textureSampler : register(s0);

/// ///////////////////////////////////////////////////
/// objects
/// ///////////////////////////////////////////////////

static const float2 kScreenSize = float2(1280.0f, 720.0f);


/// ///////////////////////////////////////////////////
/// fuinctions
/// ///////////////////////////////////////////////////

/// 指定した範囲内にポイントがあるかチェック
bool CheckInside(float3 _point, float3 _min, float3 _max) {
	return (_point.x >= _min.x && _point.y >= _min.y && _point.z >= _min.z &&
			_point.x <= _max.x && _point.y <= _max.y && _point.z <= _max.z);
}

/// ただのAABBと球の当たり判定
bool CheckSphereAABB(float3 _sphereCenter, float _sphereRadius, float3 _aabbMin, float3 _aabbMax) {
	float3 closestPoint = clamp(_sphereCenter, _aabbMin, _aabbMax);
	float distanceSq = dot(closestPoint - _sphereCenter, closestPoint - _sphereCenter);
	return distanceSq <= (_sphereRadius * _sphereRadius);
}


float3 ScreenToWorldRay(float2 _screenPos) {
	float4 clipPos = float4(_screenPos * 2.0f - 1.0f, 0, 1);
	float4 viewPos = mul(clipPos, InverseMatrix(viewProjection.matProjection));
	viewPos /= viewPos.w;
	float4 worldPos = mul(viewPos, InverseMatrix(viewProjection.matView));
	worldPos /= worldPos.w;

	float3 rayDir = normalize(worldPos.xyz - camera.position.xyz);
	return rayDir;
}


[numthreads(256, 1, 1)]
void main(
    uint3 DTid : SV_DispatchThreadID,
    uint groupIndex : SV_GroupIndex) {
	
    /// 超過していたら抜ける
	uint chunkIndex = DTid.x;
	if (voxelTerrainInfo.maxChunkCount <= chunkIndex) {
		return;
	}

    /// マウスのスクリーン座標をUVに変換してワールド座標をサンプリング
	float2 mouseUV = inputInfo.screenMousePos / kScreenSize;
	float4 mouseWorldPos = worldPositionTexture.Sample(textureSampler, mouseUV);
	
	/// 地形のローカル座標に変換
	float3 terrainLocalMousePos = mouseWorldPos.xyz - voxelTerrainInfo.terrainOrigin;
	/// チャンクの原点を計算
	float3 chunkOrigin = float3(
		(chunkIndex % voxelTerrainInfo.chunkCountXZ.x) * voxelTerrainInfo.chunkSize.x,
		0,
		(chunkIndex / voxelTerrainInfo.chunkCountXZ.x) * voxelTerrainInfo.chunkSize.z
	);

	/// マウス位置 + 半径 での球とチャンクの当たり判定
	//if (!CheckSphereAABB(
	//	mouseWorldPos.xyz, editorInfo.brushRadius,
	//	chunkOrigin, voxelTerrainInfo.chunkSize)) {
	//	return;
	//}

	/// ---------------------------------------------------
	/// ここから実際に編集する処理
	/// ---------------------------------------------------

	/// 対応するチャンクの情報
	Chunk chunk = chunks[chunkIndex];
	/// マウスのチャンク内でのローカル位置
	float3 chunkLocalMousePos = terrainLocalMousePos - chunkOrigin;
	if (!CheckInside(chunkLocalMousePos, float3(0, 0, 0), voxelTerrainInfo.chunkSize)) {
		return;
	}
	
	
	/// ローカル位置をカメラ方向に -1 して１つ前のボクセル位置にする
	float3 toCameraDire = normalize(camera.position.xyz - mouseWorldPos.xyz);
	
	float posY= chunkLocalMousePos.y / voxelTerrainInfo.textureSize.y;
	posY -= 1.0f;
	posY = abs(posY);
	posY *= voxelTerrainInfo.textureSize.y;
	
	/// ボクセル位置の色を取得
	uint3 voxelPos = uint3(chunkLocalMousePos);
	voxelPos.y = posY;
	
	voxelPos -= toCameraDire;
	
	float4 voxelColor = voxelTextures[chunk.textureId][voxelPos];

	if (voxelColor.a != 0.0f) {
		voxelTextures[chunk.textureId][voxelPos] = float4(0, 1, 0, voxelColor.a);
	}
	
	
	/// 操作次第で色を変更
	if (inputInfo.mouseLeftButton == 1) {
		if (inputInfo.keyboardLShift == 1) {
			// ----- 押し下げ ----- //
			voxelColor.a = 0.0f;
		} else {
			// ----- 押し上げ ----- //
			voxelColor.a = 1.0f;
		}
	
		voxelTextures[chunk.textureId][voxelPos] = voxelColor;
	}

}