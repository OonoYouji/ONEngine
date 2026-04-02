
struct Brush {
    float4 brushColor;
    float2 position;
    float radius;
};

static const float2     kTextureSize            = float2(1920.0f, 1080.0f);
static const float2     kScreenSize             = float2(1280.0f, 720.0f);
// static const float4     kBrushColor             = float4(0.1, 0.1, 0.7, 1.0f);

/// constant buffer
ConstantBuffer<Brush>   brush                   : register(b0);
/// texture buffers
Texture2D<float4>       gColorTexture           : register(t0);
Texture2D<float4>       gWorldPositionTexture   : register(t1);
Texture2D<float4>       gFlagsTexture           : register(t2);
RWTexture2D<float4>     gOutputTexture          : register(u0);
SamplerState            gTextureSampler         : register(s0);

[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    
    /// 必要なデータのサンプリング
	float2 texCoord = float2(DTid.xy + 0.5f) / kTextureSize;
    float4 flags    = gFlagsTexture.Sample(gTextureSampler, texCoord);
    float4 color    = gColorTexture.Sample(gTextureSampler, texCoord);

    /// entityでない場合は処理しない
    if ((int)flags.y == 0) {
        gOutputTexture[DTid.xy] = color;
        return;
    }

    float4 position = gWorldPositionTexture.Sample(gTextureSampler, texCoord);
    /// マウス位置のサンプリング
	float4 mousePos = gWorldPositionTexture.Sample(gTextureSampler, brush.position / kScreenSize);
    /// ブラシの影響範囲内かどうか
    float  dist     = distance(position.xyz, mousePos.xyz);
    if (dist < brush.radius) {
		gOutputTexture[DTid.xy] = color * brush.brushColor;
    } else {
        gOutputTexture[DTid.xy] = color; /// 元の色をそのまま出力
    }
}