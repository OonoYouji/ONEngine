#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "../../ConstantBufferData/Transform.hlsli"

struct VSInput {
    float4 position : POSITION0;
};

struct PSInput {
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION0;
};

ConstantBuffer<ViewProjection> cViewProjection : register(b0);
ConstantBuffer<Camera>         cCamera         : register(b1);

static matrix gGridWorldMatrix = {
    1000, 0, 0, 0,
    0, 1000, 0, 0,
    0, 0, 1000, 0,
    0, 0, 0, 1
};