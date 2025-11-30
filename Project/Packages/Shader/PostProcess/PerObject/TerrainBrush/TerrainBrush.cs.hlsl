
struct Brush {
    float2 position;
    float radius;
};


static const float2 kTextureSize = float2(1920.0f, 1080.0f);
static const float2 kScreenSize  = float2(1280.0f, 720.0f);
static const float4 kBrushColor  = float4(0, 0, 0, 1);

/// constant buffer
ConstantBuffer<Brush> brush : register(b0);

/// texture buffers
Texture2D<float4> colorTex    : register(t0);
Texture2D<float4> positionTex : register(t1);
Texture2D<float4> flagsTex    : register(t2);
RWTexture2D<float4> outputTex : register(u0);
SamplerState textureSampler   : register(s0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    
    /// 必要なデータのサンプリング
	float2 texCoord = float2(DTid.xy + 0.5f) / kTextureSize;
    float4 flags    = flagsTex.Sample(textureSampler, texCoord);
    float4 color    = colorTex.Sample(textureSampler, texCoord);

    /// entityでない場合は処理しない
    if ((int)flags.y == 0) {
        outputTex[DTid.xy] = color;
        return;
    }

    float4 position = positionTex.Sample(textureSampler, texCoord);
    /// マウス位置のサンプリング
	float4 mousePos = positionTex.Sample(textureSampler, brush.position / kScreenSize);
    /// ブラシの影響範囲内かどうか
    float  dist     = distance(position.xyz, mousePos.xyz);
    if (dist < brush.radius) {
		outputTex[DTid.xy] = color * kBrushColor;
    } else {
        outputTex[DTid.xy] = color; /// 元の色をそのまま出力
    }
}