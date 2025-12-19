
struct PickingParams {
    float2 mousePosNorm;
};

struct Picking {
    int entityId;
};

ConstantBuffer<PickingParams> gPickingParams : register(b0);
RWStructuredBuffer<Picking>   gPickingOutput : register(u0);

Texture2D<float4>             flagsTexture   : register(t0);
SamplerState                  samplerState   : register(s0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

    float2 uv    = gPickingParams.mousePosNorm;
    float4 flags = flagsTexture.SampleLevel(samplerState, uv, 0);

    Picking picking;
    picking.entityId = (int)flags.y;

    gPickingOutput[0] = picking;
}