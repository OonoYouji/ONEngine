#include "River.hlsli"

PSOutput main(VSOutput input) { 
    PSOutput output;

    output.color = float4(0, 0.5, 1, 1);
    output.wPosition = input.wPosition;
    output.normal = float4(input.normal, 1);
    output.flags = float4(1, 0, 0, 1);

    return output;
}