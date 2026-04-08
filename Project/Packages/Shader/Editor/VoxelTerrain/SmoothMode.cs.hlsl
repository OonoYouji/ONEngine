
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
        
        // ----- 全体を平均化して均す (Smooth) ----- //
        float sumDensity = 0.0f;
        int sampleCount = 0;
        int blurRadius = 2; // 半径2 (5x5x5)
        
        // 現在のチャンクのX, Zグリッド座標を計算
        int currentChunkX = chunkID.value % voxelTerrainInfo.chunkCountXZ.x;
        int currentChunkZ = chunkID.value / voxelTerrainInfo.chunkCountXZ.x;

        for (int z = -blurRadius; z <= blurRadius; ++z) {
            for (int y = -blurRadius; y <= blurRadius; ++y) {
                for (int x = -blurRadius; x <= blurRadius; ++x) {
                    
                    int sampleX = voxelPos.x + x;
                    int sampleY = voxelPos.y + y;
                    int sampleZ = voxelPos.z + z;
                    
                    // 【1】Y軸のチェック
                    // Y方向はチャンク分割されていない前提のため、単純に上下の範囲外を弾く
                    // （元の CheckInside の条件に合わせて 1 ～ textureSize.y - 2 を有効範囲とします）
                    if (sampleY < 1 || sampleY >= voxelTerrainInfo.textureSize.y - 1) {
                        continue;
                    }

                    int targetChunkX = currentChunkX;
                    int targetChunkZ = currentChunkZ;
                    int wrapX = sampleX;
                    int wrapZ = sampleZ;

                    // 【2】X軸のチャンク跨ぎ判定と座標ラップ
                    if (wrapX < 0) {
                        targetChunkX -= 1; // 左のチャンクへ
                        wrapX += voxelTerrainInfo.textureSize.x;
                    } else if (wrapX >= voxelTerrainInfo.textureSize.x) {
                        targetChunkX += 1; // 右のチャンクへ
                        wrapX -= voxelTerrainInfo.textureSize.x;
                    }

                    // 【3】Z軸のチャンク跨ぎ判定と座標ラップ
                    if (wrapZ < 0) {
                        targetChunkZ -= 1; // 奥のチャンクへ
                        wrapZ += voxelTerrainInfo.textureSize.z;
                    } else if (wrapZ >= voxelTerrainInfo.textureSize.z) {
                        targetChunkZ += 1; // 手前のチャンクへ
                        wrapZ -= voxelTerrainInfo.textureSize.z;
                    }

                    // 【4】対象チャンクがワールド(地形)の範囲内に存在するかチェック
                    // ※ voxelTerrainInfo.chunkCountXZ.y がZ方向のチャンク総数であると仮定しています
                    if (targetChunkX >= 0 && targetChunkX < voxelTerrainInfo.chunkCountXZ.x &&
                        targetChunkZ >= 0 && targetChunkZ < voxelTerrainInfo.chunkCountXZ.y) {
                        
                        // 対象のチャンクIDを再計算
                        int targetChunkID = targetChunkZ * voxelTerrainInfo.chunkCountXZ.x + targetChunkX;
                        
                        // 対象チャンクのテクスチャから、ラップした座標を使って密度を取得
                        int3 finalSamplePos = int3(wrapX, sampleY, wrapZ);
                        sumDensity += voxelTextures[chunks[targetChunkID].textureId][finalSamplePos].a;
                        sampleCount++;
                    }
                }
            }
        }
        
        if (sampleCount > 0) {
            float avgDensity = sumDensity / (float)sampleCount;
            voxelColor.a = lerp(voxelColor.a, avgDensity, saturate(val));
            voxelTextures[chunks[chunkID.value].textureId][voxelPos] = voxelColor;
        }
    }
}