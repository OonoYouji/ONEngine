struct LODInfo {
    uint32_t useLod;
    float32_t lodDistance1;
    float32_t lodDistance2;
    float32_t lodDistance3;
    int32_t lodLevel0;
    int32_t lodLevel1;
    int32_t lodLevel2;
    int32_t lodLevel3;
    float32_t maxDrawDistance;
    uint32_t lod;
};

ConstantBuffer<LODInfo> lodInfo : register(b3);


static const uint32_t TRANSITION_NX = 0x01; // -X (Left)
static const uint32_t TRANSITION_PX = 0x02; // +X (Right)
static const uint32_t TRANSITION_NY = 0x04; // -Y (Bottom)
static const uint32_t TRANSITION_PY = 0x08; // +Y (p)
static const uint32_t TRANSITION_NZ = 0x10; // -Z (Back)
static const uint32_t TRANSITION_PZ = 0x20; // +Z (Front)
static const uint32_t TRANSITION_PXZ = 0x40; // +X +Z
static const uint32_t TRANSITION_NXZ = 0x80; // -X -Z
static const uint32_t TRANSITION_NXPZ = 0x100; // -X +Z
static const uint32_t TRANSITION_PXNZ = 0x200; // +X -Z



// -----------------------------------------------------------------------------
// ヘルパー：LOD計算関数
// -----------------------------------------------------------------------------
uint32_t GetLOD(float32_t distanceToCamera)
{
    if (distanceToCamera < lodInfo.lodDistance1) {
        return lodInfo.lodLevel0;
    } else if (distanceToCamera < lodInfo.lodDistance2) {
        return lodInfo.lodLevel1;
    } else if (distanceToCamera < lodInfo.lodDistance3) {
        return lodInfo.lodLevel2;
    } 
    return lodInfo.lodLevel3;
}

uint32_t CalculateLOD(float32_t3 worldPos, float32_t3 cameraPos)
{
    return GetLOD(distance(worldPos, cameraPos));
}

uint32_t GetSubChunkSize(uint32_t lodLevel)
{
    return 1u << lodLevel;
}

// -----------------------------------------------------------------------------
// chunkCenter: 現在処理中のチャンクの中心ワールド座標
// chunkSize  : 現在処理中のチャンクの一辺のサイズ (LOD0基準ではなく、現在のスケールでのサイズ)
// myLOD      : 現在のチャンクのLODレベル
// cameraPos  : カメラ位置
// -----------------------------------------------------------------------------
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
        
        // カメラから隣接AABBへの最近点 (nearPos) を計算
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
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax);
        
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
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax);
        
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
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax);
        
        uint32_t lodPZ = CalculateLOD(nNearPos, cameraPos);
        if (lodPZ > myLOD) mask |= TRANSITION_PZ;
    }

    // ------------------------------------------------------
    // +X +Z 方向
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter + float32_t3(offset.x, 0, offset.z);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax);
        
        uint32_t lodPXZ = CalculateLOD(nNearPos, cameraPos);
        if (lodPXZ > myLOD) mask |= TRANSITION_PXZ;
    }

    // ------------------------------------------------------
    // -X -Z 方向
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter - float32_t3(offset.x, 0, offset.z);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax);
        
        uint32_t lodNXZ = CalculateLOD(nNearPos, cameraPos);
        if (lodNXZ > myLOD) mask |= TRANSITION_NXZ;
    }

    // ------------------------------------------------------
    // -X +Z 方向
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter + float32_t3(-offset.x, 0, offset.z);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax);
        
        uint32_t lodNXPZ = CalculateLOD(nNearPos, cameraPos);
        if (lodNXPZ > myLOD) mask |= TRANSITION_NXPZ;
    }

    // ------------------------------------------------------
    // +X -Z 方向
    // ------------------------------------------------------
    {
        float32_t3 neighborCenter = chunkCenter + float32_t3(offset.x, 0, -offset.z);
        
        float32_t3 nMin = neighborCenter - halfSize;
        float32_t3 nMax = neighborCenter + halfSize;
        float32_t3 nNearPos = clamp(cameraPos, nMin, nMax);
        
        uint32_t lodPXNZ = CalculateLOD(nNearPos, cameraPos);
        if (lodPXNZ > myLOD) mask |= TRANSITION_PXNZ;
    }

    return mask;
}
