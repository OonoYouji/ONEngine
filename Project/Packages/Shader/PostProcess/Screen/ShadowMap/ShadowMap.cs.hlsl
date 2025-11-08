#include "../../../ConstantBufferData/ViewProjection.hlsli"


struct ShadowApplyParameters {
    float2 screenSize;
    float2 texelSizeShadow;
    float shadowBias;
    float shadowDarkness;
    int pcfRadius;
    float padding;
};


/// light camera
ConstantBuffer<ViewProjection> viewProjection : register(b0);

/// shadow apply parameters
ConstantBuffer<ShadowApplyParameters> shadowParams : register(b1);

/// 
Texture2D<float4> sceneTexture : register(t0);  /// sceneのカラーテクスチャ
Texture2D<float> depthTexture : register(t1);   /// cameraの深度テクスチャ
Texture2D<float> shadowMap : register(t2);      /// light cameraの深度テクスチャ
Texture2D<float4> worldTexture : register(t3);  /// world position texture
SamplerState linearSampler : register(s0);
SamplerComparisonState shadowSampler : register(s1);

RWTexture2D<float4> outputTexture : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint2 px = DTid.xy;

    /// 範囲外チェック
    if(px.x >= shadowParams.screenSize.x || px.y >= shadowParams.screenSize.y) {
        return;
    }


    /// UV座標計算(pixelの中心をサンプリングできるように 0.5を加算)
    float2 uv = (float2(px) + 0.5) / shadowParams.screenSize;

    float4 sceneColor = sceneTexture.Sample(linearSampler, uv);
    float3 worldPos = worldTexture.Sample(linearSampler, uv).xyz;


    float4 lightH = mul(float4(worldPos, 1.0), viewProjection.matVP);
    lightH.xyz /= lightH.w;

    float2 shadowUV = lightH.xy * 0.5 + 0.5;
    float shadowDepth = lightH.z * 0.5 + 0.5;

    /// シャドウ比較+PCF
    float shadowSum = 0.0;
    int count = 0;

    for(int y = -shadowParams.pcfRadius; y <= shadowParams.pcfRadius; ++y) {
        for(int x = -shadowParams.pcfRadius; x <= shadowParams.pcfRadius; ++x) {

            float2 offset = float2(x,y) * shadowParams.texelSizeShadow;
            float cmp = shadowMap.SampleCmpLevelZero(shadowSampler, shadowUV + offset, shadowDepth - shadowParams.shadowBias);
            shadowSum += cmp;
            count++;
        }
    }

    float shadow = shadowSum / count;

    /// シャドウ適用
    float shadowFactor = lerp(1.0f, 1.0f - shadowParams.shadowDarkness, 1.0f - shadow);
    float3 finalColor = sceneColor.rgb * shadowFactor;

    outputTexture[px] = float4(finalColor, 1.0);

}