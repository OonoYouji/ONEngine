struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

// 頂点バッファなしで全画面三角形を生成する
VSOutput vs_main(uint vID : SV_VertexID) {
    VSOutput output;
    output.uv = float2((vID << 1) & 2, vID & 2);
    output.position = float4(output.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}

Texture2D gMainTexture : register(t0);
SamplerState gSampler : register(s0);

float4 ps_main(VSOutput input) : SV_TARGET {
    float4 color = gMainTexture.Sample(gSampler, input.uv);
    
    // シンプルなトーンマッピング (暫定)
    // 今後ここを拡張して本格的なACESトーンマッピング等を実装します
    color.rgb = color.rgb / (color.rgb + 1.0);
    
    return color;
}
