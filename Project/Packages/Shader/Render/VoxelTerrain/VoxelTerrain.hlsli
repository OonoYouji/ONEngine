
/// ---------------------------------------------------
/// Structs
/// ---------------------------------------------------

struct VertexOut {
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION0;
    float3 normal : NORMAL0;
};


struct Chunk {
    /// Texture3D Id
    uint textureId;       
};

struct VoxelTerrainInfo {
    float3 chunkSize;
};


struct Payload {

};