#include "Common.hlsli"


/// /////////////////////////////////////////////////////////////////////////////\
/// マテリアルの編集、ボクセルのRをテクスチャID、Gをテクスチャの重みとして使用するモード
/// /////////////////////////////////////////////////////////////////////////////

[shader("compute")]
[numthreads(10, 10, 10)]
void main(
    uint3 DTid : SV_DispatchThreadID) {

	/// 地形のローカル座標に変換
    float3 mouseWorldPos = mousePosBuffer[0].worldPos.xyz;
	float3 terrainLocalMousePos = mouseWorldPos - voxelTerrainInfo.terrainOrigin;
	/// チャンクの原点を計算
	float3 chunkOrigin = float3(
		(chunkID.value % voxelTerrainInfo.chunkCountXZ.x) * voxelTerrainInfo.chunkSize.x,
		0,
		(chunkID.value / voxelTerrainInfo.chunkCountXZ.x) * voxelTerrainInfo.chunkSize.z
	);

	/// ---------------------------------------------------
	/// ここから実際に編集する処理
	/// ---------------------------------------------------

	/// 対応するチャンクの情報
	/// マウスのチャンク内でのローカル位置
	float3 chunkLocalMousePos = terrainLocalMousePos - chunkOrigin;
	
    /// Y軸を反転させ左手座標系からテクスチャ座標系に
	float posY = chunkLocalMousePos.y / voxelTerrainInfo.textureSize.y;
	posY -= 1.0f;
	posY = abs(posY);
	posY *= voxelTerrainInfo.textureSize.y;
	chunkLocalMousePos.y = posY;
	
	uint32_t radius = (uint32_t) editorInfo.brushRadius;
	int3 lpos = int32_t3(DTid - int3(radius, radius, radius));

    // 距離計算
    float dist = length((float3)lpos);

    // 範囲外はそのまま除外
    if (dist > radius) { return; }

    // 中心:1.0 → 外周:0.0
    float falloff = 1.0f - (dist / radius);
    float val = editorInfo.brushStrength * falloff;

	/// ボクセル位置を取得
	int3 voxelPos = chunkLocalMousePos + lpos;
	
	/// 範囲外チェック
	if (!CheckInside(voxelPos, int3(0, 1, 0), int3(voxelTerrainInfo.textureSize) - int3(0, 1, 0))) {
        return;
	}

    float4 voxelColor = voxelTextures[chunks[chunkID.value].textureId][voxelPos];
	/// 操作次第で色を変更
	if (inputInfo.mouseLeftButton == 1) {
        uint id = editorInfo.materialId;
		if (inputInfo.keyboardLShift == 1) {
			// ----- 押し下げ ----- //
			voxelColor[id] -= val;
			if(voxelColor[id] < 0.0f) {
                voxelColor[id] = 0.0f;
			}
		} else {
			// ----- 押し上げ ----- //
			voxelColor[id] += val;
            if(voxelColor[id] > 1.0f) {
                voxelColor[id] = 1.0f;
			}
		}
    
		voxelTextures[chunks[chunkID.value].textureId][voxelPos] = voxelColor;
	}
}