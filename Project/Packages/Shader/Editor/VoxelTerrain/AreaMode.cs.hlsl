#include "Common.hlsli"

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
	if (lpos.x * lpos.x + lpos.y * lpos.y + lpos.z * lpos.z > radius * radius) {
        return;
	}

	/// ボクセル位置の色を取得
	int3 voxelPos = chunkLocalMousePos + lpos;
	
	/// 範囲外チェック
	if (!CheckInside(voxelPos, int3(0, 1, 0), int3(voxelTerrainInfo.textureSize) - int3(0, 1, 0))) {
        return;
	}

	
	float4 voxelColor = voxelTextures[chunks[chunkID.value].textureId][voxelPos];
	
	/// 操作次第で色を変更
	if (inputInfo.mouseLeftButton == 1) {
        float val = editorInfo.brushStrength;
		if (inputInfo.keyboardLShift == 1) {
			// ----- 押し下げ ----- //
			voxelColor.a -= val;
            if(voxelColor.a < 0.0f) {
                voxelColor.a = 0.0f;
            }
		} else {
			// ----- 押し上げ ----- //
			voxelColor.a += val;
            if(voxelColor.a > 1.0f) {
                voxelColor.a = 1.0f;
            }
		}
    
		voxelTextures[chunks[chunkID.value].textureId][voxelPos] = voxelColor;
	}

}

