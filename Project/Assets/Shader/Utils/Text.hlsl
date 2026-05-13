struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint instanceID : SV_InstanceID;
};

#include "../Schema/Buffers.hlsli"

ConstantBuffer<SceneData> gSceneData : register(b0);
StructuredBuffer<TextData> gChars : register(t0);

// --- Bindless ---
Texture2D gTextures[] : register(t0, space1);
SamplerState gSampler : register(s0);

VSOutput vs_main(uint vID : SV_VertexID, uint iID : SV_InstanceID) {
    VSOutput output;
    TextData data = gChars[iID];
    
    // クアッド頂点 (0: TL, 1: TR, 2: BL, 3: BR)
    float2 uv = float2(vID % 2, vID / 2);
    float2 pos = uv * 2.0f - 1.0f;
    pos.y *= -1.0f;
    
    // 事前に計算されたUV範囲を適用
    float2 finalUV = lerp(data.uvMin, data.uvMax, uv);
    
    output.position = mul(float4(pos, 0.0f, 1.0f), data.world);
    output.position = mul(output.position, gSceneData.viewProj);
    output.uv = finalUV;
    output.instanceID = iID;
    
    return output;
}

struct PSOutput {
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    uint2 idFlags : SV_Target2;
};

PSOutput ps_main(VSOutput input) {
    TextData data = gChars[input.instanceID];
    
    // フォントテクスチャは R8_UNORM (アルファ値として使用)
    float alpha = gTextures[NonUniformResourceIndex(data.textureIndex)].Sample(gSampler, input.uv).r;
    
    // 透明度が低すぎる部分はカット (テクスチャの端のノイズ対策)
    if (alpha < 0.1f) discard;
    
    PSOutput output;
    output.color = float4(data.color.rgb, alpha * data.color.a);
    output.normal = float4(0.5f, 0.5f, 1.0f, 1.0f);
    output.idFlags = uint2(data.entityID, 0);
    return output;
}
