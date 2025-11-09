#include "../../../ConstantBufferData/ViewProjection.hlsli"
#include "../../../ConstantBufferData/Material.hlsli"

/// ===================================================
/// シャドウ適用用パラメータ
/// ===================================================
struct ShadowApplyParameters {
    float2 screenSize;        // 出力テクスチャのサイズ (例: 1920x1080)
    float2 texelSizeShadow;   // ShadowMap の 1ピクセルサイズ (1/width, 1/height)
    float shadowBias;         // バイアス（アクネ防止）
    float shadowDarkness;     // 影の濃さ (0〜1)
    int   pcfRadius;          // PCFサンプリング半径
    float padding[3];         // アラインメント調整（16バイト境界）
};

/// ===================================================
/// 定数バッファ
/// ===================================================
ConstantBuffer<ViewProjection> viewProjection : register(b0); // ライトカメラのViewProjection行列
ConstantBuffer<ShadowApplyParameters> shadowParams : register(b1);

/// ===================================================
/// 入出力テクスチャ
/// ===================================================
Texture2D<float4> sceneTexture : register(t0);  // シーンカラー (メインカメラ)
Texture2D<float>  shadowMap    : register(t1);  // ライトカメラDepth
Texture2D<float4> worldTexture : register(t2);  // ワールド座標（メインカメラ）
Texture2D<float4> flagsTexture : register(t3);  // フラグ情報（メインカメラ）

SamplerState linearSampler : register(s0);
SamplerComparisonState shadowSampler : register(s1);

RWTexture2D<float4> outputTexture : register(u0);

/// ===================================================
/// メインスレッド
/// ===================================================
[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint2 px = DTid.xy;

    // 画面外チェック
    if (px.x >= (uint)shadowParams.screenSize.x || px.y >= (uint)shadowParams.screenSize.y) {
        return;
    }

    float2 uv = (float2(px) + 0.5) / shadowParams.screenSize;

    // 入力カラーとワールド座標取得
    float4 sceneColor = sceneTexture.Load(int3(px, 0));
    float3 worldPos   = worldTexture.Load(int3(px, 0)).xyz;
    float4 flags      = flagsTexture.Load(int3(px, 0));

    /// 影を落とさないフラグが立っている場合は処理をスキップ
    if (IsPostEffectEnabled(flags.x, PostEffectFlags_Shadow) == false) {
        outputTexture[px] = sceneColor;
        return;
    }

    // ワールド → ライト空間変換
    float4 lightClip = mul(float4(worldPos, 1.0), viewProjection.matVP);

    // w=0は除外（例えば背景など）
    if (abs(lightClip.w) < 1e-5f) {
        outputTexture[px] = sceneColor;
        return;
    }


    // NDC(-1~1) → ShadowMapUV(0~1)
    lightClip /= lightClip.w;
    float2 shadowUV = lightClip.xy * 0.5f + 0.5f;
    shadowUV.y = 1.0f - shadowUV.y; // V反転
    float shadowDepth = lightClip.z;



    // 範囲外なら影を計算しない
    if (any(shadowUV < 0.0f) || any(shadowUV > 1.0f)) {
        outputTexture[px] = sceneColor;
        return;
    }

    // 従来のPCFループはコメントアウト
    // float litSum = 0.0;
    // int sampleCount = 0;
    // for (...) { ... }

    float cmp = shadowMap.SampleCmpLevelZero(shadowSampler, shadowUV, shadowDepth - shadowParams.shadowBias);
    // lit = 1なら光、0なら影
    float lit = cmp;

    // シャドウ適用
    float shadowFactor = lerp(1.0f, 1.0f - shadowParams.shadowDarkness, 1.0f - lit);
    float3 finalColor = sceneColor.rgb * shadowFactor;

    outputTexture[px] = float4(finalColor, sceneColor.a);


    // float depth = shadowMap.SampleLevel(linearSampler, shadowUV, 0);
    // outputTexture[px] = float4(depth, depth, depth, 1.0f);

    // float4 lightPos = mul(float4(worldPos, 1.0), viewProjection.matVP);
    // outputTexture[px] = float4(lightPos.xyz * 0.5 + 0.5, 1.0f);

}
