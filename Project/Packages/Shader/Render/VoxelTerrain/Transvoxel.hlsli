struct VertexOut {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD0;
};

struct Payload {
    uint lod;
};