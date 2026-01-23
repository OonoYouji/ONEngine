struct Chunk {
	uint textureId;
};

struct VoxelTerrainInfo {
	float32_t3 terrainOrigin;
	uint32_t3 textureSize;
	uint32_t3 chunkSize;
	uint32_t2 chunkCountXZ;
	uint32_t maxChunkCount;
    float32_t isoLevel;
};

struct SubChunk {
	uint32_t3 subChunkOrigin;
	uint32_t3 subChunkSize;
};

// 頂点オフセット (0-7) - 中心を原点とする版
static const float3 kCornerOffsets[8] = {
	float3(-0.5, -0.5, -0.5), float3(0.5, -0.5, -0.5),
	float3(0.5, 0.5, -0.5), float3(-0.5, 0.5, -0.5),
	float3(-0.5, -0.5, 0.5), float3(0.5, -0.5, 0.5),
	float3(0.5, 0.5, 0.5), float3(-0.5, 0.5, 0.5)
};

// エッジ接続情報
static const int2 kEdgeConnection[12] = {
	int2(0, 1), int2(1, 2), int2(2, 3), int2(3, 0), // Bottom
	int2(4, 5), int2(5, 6), int2(6, 7), int2(7, 4), // Top
	int2(0, 4), int2(1, 5), int2(2, 6), int2(3, 7) // Vertical
};

static const uint32_t TRANSITION_NX = 0x01; // -X (Left)
static const uint32_t TRANSITION_PX = 0x02; // +X (Right)
static const uint32_t TRANSITION_NY = 0x04; // -Y (Bottom)
static const uint32_t TRANSITION_PY = 0x08; // +Y (p)
static const uint32_t TRANSITION_NZ = 0x10; // -Z (Back)
static const uint32_t TRANSITION_PZ = 0x20; // +Z (Front)



/// ---------------------------------------------------
/// functions
/// ---------------------------------------------------

uint32_t IndexOfMeshGroup(uint32_t3 groupID, uint32_t3 dim) {
	return groupID.x
         + groupID.y * dim.x
         + groupID.z * (dim.x * dim.y);
}




// -----------------------------------------------------------------------------
// ヘルパー：LOD計算関数
// -----------------------------------------------------------------------------
uint CalculateLOD(float32_t3 worldPos, float32_t3 cameraPos)
{
    float dist = distance(worldPos, cameraPos);
    
    // 例: 距離ベースの簡易LOD
    // 実際にはもっと細かく調整するか、事前計算されたLODマップを参照します
    if (dist < 50.0f)  return 0;
    if (dist < 100.0f) return 1;
    if (dist < 200.0f) return 2;
    return 3;
}

// -----------------------------------------------------------------------------
// chunkCenter: 現在処理中のチャンクの中心ワールド座標
// chunkSize  : 現在処理中のチャンクの一辺のサイズ (LOD0基準ではなく、現在のスケールでのサイズ)
// myLOD      : 現在のチャンクのLODレベル
// cameraPos  : カメラ位置
// -----------------------------------------------------------------------------
// 修正: chunkSizeを float -> float32_t3 に変更
uint32_t GetTransitionMask(float32_t3 chunkCenter, float32_t3 chunkSize, uint32_t myLOD, float32_t3 cameraPos)
{
    uint32_t mask = 0;
    
    // 判定用のヘルパー関数（ラムダ式が使えないので直接計算）
    // 隣接チャンクの中心位置とサイズから、そのチャンクの「カメラ最近点」を求めてLOD計算する
    
    float32_t3 halfSize = chunkSize / 2.0f;
    float32_t3 offset = chunkSize; // 隣接へのオフセット（サイズ分だけ移動）

    // ------------------------------------------------------
    // X軸方向 (-X: Left)
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter - float32_t3(offset.x, 0, 0);
        
        // 隣接チャンクのAABB範囲
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        
        // カメラから隣接AABBへの最近点 (nearPos) を計算 ★ここが修正ポイント
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax);
        
        uint32_t lodNX = CalculateLOD(nNearPos, cameraPos);
        if (lodNX > myLOD) mask |= TRANSITION_NX;
    }

    // ------------------------------------------------------
    // X軸方向 (+X: Right)
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter + float32_t3(offset.x, 0, 0);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax); // ★最近点を使う
        
        uint32_t lodPX = CalculateLOD(nNearPos, cameraPos);
        if (lodPX > myLOD) mask |= TRANSITION_PX;
    }

    // ------------------------------------------------------
    // Z軸方向 (-Z: Back)
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter - float32_t3(0, 0, offset.z);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax); // ★最近点を使う
        
        uint32_t lodNZ = CalculateLOD(nNearPos, cameraPos);
        if (lodNZ > myLOD) mask |= TRANSITION_NZ;
    }

    // ------------------------------------------------------
    // Z軸方向 (+Z: Front)
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter + float32_t3(0, 0, offset.z);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax); // ★最近点を使う
        
        uint32_t lodPZ = CalculateLOD(nNearPos, cameraPos);
        if (lodPZ > myLOD) mask |= TRANSITION_PZ;
    }

    return mask;
}
