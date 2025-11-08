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
    if (px.x >= (uint)shadowParams.screenSize.x || px.y >= (uint)shadowParams.screenSize.y) return;

    float2 uv = (float2(px) + 0.5) / shadowParams.screenSize;

    float4 sceneColor = sceneTexture.Sample(linearSampler, uv);
    float3 worldPos = worldTexture.Sample(linearSampler, uv).xyz;

    // ライト空間へ変換（world -> view -> proj）
    float4 lightH = mul(float4(worldPos, 1.0), viewProjection.matVP);

    // ホモジニアス除算（w成分まで割る）
    lightH /= lightH.w;

    // UVに変換（NDC -> [0,1]）
    float2 shadowUV = lightH.xy * 0.5 + 0.5;

    // ----- Y反転が必要ならここを有効化して試す -----
    // shadowUV.y = 1.0 - shadowUV.y;
    // ----------------------------------------------

    // Depthの扱い（DirectXの標準：NDC.zは[0,1]なのでそのまま使う）
    float shadowDepth = lightH.z;

    // 範囲外チェック: シャドウマップ外は lit とする（または好みで shadow）
    if (shadowUV.x < 0.0 || shadowUV.x > 1.0 || shadowUV.y < 0.0 || shadowUV.y > 1.0) {
        outputTexture[px] = sceneColor; // そのまま lit
        return;
    }

    // PCF サンプリング
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
    float lit = litSum / count; // 1 = lit, 0 = shadow

    float shadowFactor = lerp(1.0f, 1.0f - shadowParams.shadowDarkness, 1.0f - lit);
    float3 finalColor = sceneColor.rgb * shadowFactor;
    outputTexture[px] = float4(finalColor, sceneColor.a);

    // outputTexture[px] = float4( saturate(lightH.xyz * 0.5 + 0.5), 1.0 );

}
