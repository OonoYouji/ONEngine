#include "../../../ConstantBufferData/Material.hlsli"
#include "../../../ConstantBufferData/ViewProjection.hlsli"

struct FogParams {
    float3 fogColor;  // フォグの色（空や背景色に合わせると自然です）
    float fogStart;   // フォグがかかり始める距離
    float fogEnd;     // フォグが完全に不透明（FogColor）になる距離
};

/// Buffers
ConstantBuffer<FogParams>   gFogParams               : register(b0);
ConstantBuffer<Camera>      gCamera                  : register(b1);

Texture2D<float4>           gColorTexture            : register(t0);
Texture2D<float4>           gWorldPositionTexture    : register(t1);
RWTexture2D<float4>         gOutputTexture           : register(u0);
SamplerState                gTextureSampler          : register(s0);

/// static variables
static const float32_t2 screenSize = float32_t2(1920.0f, 1080.0f);

/// main
[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
// 画面外の処理をスキップ
    if (DTid.x >= (uint)screenSize.x || DTid.y >= (uint)screenSize.y) return;

    // ピクセル座標を直接指定してロードする (エラー回避 & 高速)
    float4 color = gColorTexture[DTid.xy];
    float4 worldPos = gWorldPositionTexture[DTid.xy];

    // ① カメラからピクセルまでの距離を計算
    float dist = length(worldPos.xyz - gCamera.position.xyz);

    // ② 距離と Start/End を使ってフォグの強さを計算 (リニアフォグ)
    // ゼロ除算を防ぐため、念のため max で微小な値を入れると安全です
    float fogRange = max(gFogParams.fogEnd - gFogParams.fogStart, 0.0001f);
    float fogFactor = saturate((dist - gFogParams.fogStart) / fogRange);

    // ③ 変数名を構造体に合わせる (gFogParams.color -> gFogParams.fogColor)
    float3 finalColor = lerp(color.rgb, gFogParams.fogColor, fogFactor);

    // 出力
    gOutputTexture[DTid.xy] = float4(finalColor, color.a);
}