// ClusteredLight.hlsli
#ifndef CLUSTERED_LIGHT_HLSLI
#define CLUSTERED_LIGHT_HLSLI

// クラスタの分割数 (16x9x24)
#define CLUSTER_GRID_X 16
#define CLUSTER_GRID_Y 9
#define CLUSTER_GRID_Z 24

// 1つのクラスタあたりの最大ライト数
#define MAX_LIGHTS_PER_CLUSTER 64

struct ClusterAABB {
    float4 minPoint; // w is padding
    float4 maxPoint; // w is padding
};

struct LightGrid {
    uint offset;
    uint count;
};

// クラスタリング用の定数バッファ
struct ClusterParams {
    float4x4 invProj;
    float nearZ;
    float farZ;
    float screenWidth;
    float screenHeight;
    uint totalLights;
};

#endif
