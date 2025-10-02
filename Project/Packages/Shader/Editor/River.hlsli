struct ControlPoint {
    float3 position;
    float width;
};

struct RiverVertex {
    float4 position;
    float2 uv;
    float3 normal;
};

struct RiverParams {
    uint totalSegments;
    uint totalVertices;
    uint totalSamples;
    uint samplePerSegment;
};