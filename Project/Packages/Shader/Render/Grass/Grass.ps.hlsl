#include "Grass.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
};

[shader("pixel")]
float4 PSMain(PSInput input) : SV_TARGET
{
    // 緑色で草を描画
    return float4(0, 1, 0, 1);
}
