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
Texture2D<float4> sceneTexture : register(t0);
Texture2D<float4> depthTexture : register(t1);
Texture2D<float4> shadowMap : register(t2);
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

    /// 深度、色を取得
    float4 sceneColor = sceneTexture.Sample(linearSampler, uv);
    float depth = depthTexture.Sample(linearSampler, uv);

}