#include "../../../ConstantBufferData/ViewProjection.hlsli"

struct ShadowApplyParameters {
    float2 screenSize;
    float2 texelSizeShadow;
    float shadowBias;
    float shadowDarkness;
    int pcfRadius;
    float padding;
};

// Light cameraのビュー射影
ConstantBuffer<ViewProjection> viewProjection : register(b0);

// 影適用パラメータ
ConstantBuffer<ShadowApplyParameters> shadowParams : register(b1);

// 入力テクスチャ
Texture2D<float4> sceneTexture : register(t0);     // Sceneカラー
Texture2D<float> shadowMap : register(t2);         // Lightカメラの深度
Texture2D<float4> worldTexture : register(t3);     // ワールド座標

SamplerState linearSampler : register(s0);
SamplerComparisonState shadowSampler : register(s1);

// 出力テクスチャ
RWTexture2D<float4> outputTexture : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint2 px = DTid.xy;

    // 画面外チェック
    if (px.x >= (uint)shadowParams.screenSize.x || px.y >= (uint)shadowParams.screenSize.y) return;

    float2 uv = (float2(px) + 0.5) / shadowParams.screenSize;

    // 入力カラー
    float4 sceneColor = sceneTexture.Sample(linearSampler, uv);

    // ワールド座標
    float3 worldPos = worldTexture.Sample(linearSampler, uv).xyz;

    // ワールド -> ライトカメラ空間 -> 射影
    float4 lightH = mul(float4(worldPos, 1.0), viewProjection.matVP);

    // ホモジニアス除算
    lightH /= lightH.w;

    // NDC -> UV ([0,1])
    float2 shadowUV = lightH.xy * 0.5 + 0.5;
    float shadowDepth = lightH.z;

    // シャドウマップ範囲外はlit
    if (shadowUV.x < 0.0 || shadowUV.x > 1.0 || shadowUV.y < 0.0 || shadowUV.y > 1.0) {
        outputTexture[px] = sceneColor;
        return;
    }

    // PCFによるシャドウ比較
    int radius = max(0, shadowParams.pcfRadius);
    float litSum = 0.0;
    int count = 0;

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            float2 offset = float2(x, y) * shadowParams.texelSizeShadow;
            float cmp = shadowMap.SampleCmpLevelZero(shadowSampler, shadowUV + offset, shadowDepth - shadowParams.shadowBias);
            litSum += cmp;
            count++;
        }
    }

    float lit = litSum / count; // 1=lit, 0=shadow

    // シャドウ適用
    float shadowFactor = lerp(1.0f, 1.0f - shadowParams.shadowDarkness, 1.0f - lit);
    float3 finalColor = sceneColor.rgb * shadowFactor;

    outputTexture[px] = float4(finalColor, sceneColor.a);
}
