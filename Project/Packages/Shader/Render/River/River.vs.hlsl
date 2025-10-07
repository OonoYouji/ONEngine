#include "River.hlsli"

#include "../../ConstantBufferData/Transform.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"

ConstantBuffer<ViewProjection> viewProjection : register(b0);

VSOutput main(VSInput input) {
    VSOutput output;

    output.position = mul(input.position, viewProjection.matVP);
    output.uv = input.uv;
    output.normal = normalize(input.normal);
    output.wPosition = input.position;

    return output;
}