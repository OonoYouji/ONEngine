#pragma once

struct VSInput {
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    nointerpolation uint instanceId : TEXCOORD1;
    float4 color : COLOR0;
};
