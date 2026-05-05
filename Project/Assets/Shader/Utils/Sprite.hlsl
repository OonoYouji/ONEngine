struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint instanceID : SV_InstanceID;
};

#include "../Schema/Schema.hlsli"

ConstantBuffer<SceneData> gSceneData : register(b0);
StructuredBuffer<SpriteData> gSprites : register(t0);

// --- Bindless ---
Texture2D gTextures[] : register(t0, space1);
SamplerState gSampler : register(s0);

VSOutput vs_main(uint vID : SV_VertexID, uint iID : SV_InstanceID) {
    VSOutput output;
    SpriteData sprite = gSprites[iID];
    
    // クアッド頂点 (0: TL, 1: TR, 2: BL, 3: BR)
    float2 uv = float2(vID % 2, vID / 2);
    float2 pos = uv * 2.0f - 1.0f; // -1 to 1
    pos.y *= -1.0f; // Flip Y
    
    output.position = mul(float4(pos, 0.0f, 1.0f), sprite.world);
    output.position = mul(output.position, gSceneData.viewProj);
    output.uv = uv;
    output.instanceID = iID;
    
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    SpriteData sprite = gSprites[input.instanceID];
    float4 texColor = gTextures[NonUniformResourceIndex(sprite.textureIndex)].Sample(gSampler, input.uv);
    return texColor * sprite.color;
}
