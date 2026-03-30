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

	/// ボクセル位置を取得
	int3 voxelPos = chunkLocalMousePos + lpos;
	
	/// 範囲外チェック
	if (!CheckInside(voxelPos, int3(0, 1, 0), int3(voxelTerrainInfo.textureSize) - int3(0, 1, 0))) {
        return;
	}

    float4 voxelColor = voxelTextures[chunks[chunkID.value].textureId][voxelPos];
	float threshold = 0.01f; // 隣接とみなすアルファ値の閾値
	bool hasAdjacent = false;

	// 自身がすでに閾値以上の場合は編集可能とする（削る処理などのため）
	if (voxelColor.a >= threshold) {
		hasAdjacent = true;
	} else {
		// 6方向（上下左右前後）のオフセット
		int3 offsets[6] = {
			int3( 1,  0,  0), int3(-1,  0,  0),
			int3( 0,  1,  0), int3( 0, -1,  0),
			int3( 0,  0,  1), int3( 0,  0, -1)
		};

		// 6方向のいずれかの隣接ボクセルが閾値以上かチェック
		for (int i = 0; i < 6; ++i) {
			int3 nPos = voxelPos + offsets[i];
			
			// 【追加】Y軸はテクスチャ座標系で反転しているため、Yが大きい方向が「下」になります [cite: 51]。
			// チャンクの一番下（底面境界）を参照した場合は、強制メッシュ化されるため隣接扱いとします。
			if (nPos.y >= voxelTerrainInfo.textureSize.y - 1) {
				hasAdjacent = true;
				break;
			}

			// 隣接ボクセルがチャンクのテクスチャ範囲内にあるか安全のためにチェック
			if (CheckInside(nPos, int3(0, 0, 0), int3(voxelTerrainInfo.textureSize) - int3(1, 1, 1))) {
				if (voxelTextures[chunks[chunkID.value].textureId][nPos].a >= threshold) {
					hasAdjacent = true;
					break; // 1つでも隣接していればOK
				}
			}
		}
	}

	// 隣接するボクセルが存在しない（かつ自身も空）なら編集処理をスキップ
	if (!hasAdjacent) {
		return;
	}
	/// ---------------------------------------------------
	
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