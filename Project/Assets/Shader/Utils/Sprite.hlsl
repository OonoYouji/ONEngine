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
    float2 localPos = uv * 2.0f - 1.0f; // -1 to 1
    localPos.y *= -1.0f; // Flip Y
    
    float4 worldPos;
    
    if (sprite.isBillboard) {
        // ビルボード計算: 
        // インスタンスのワールド行列の第3列（位置）のみを使用し、
        // カメラの視線ベクトルに合わせた回転を動的に適用する。
        
        float3 center = float3(sprite.world[3][0], sprite.world[3][1], sprite.world[3][2]);
        float3 cameraPos = gSceneData.cameraPos;
        
        float3 forward = normalize(cameraPos - center);
        float3 right = normalize(cross(float3(0, 1, 0), forward));
        float3 up = cross(forward, right);
        
        // スケールの復元 (world行列の対角要素の長さから取得)
        float scaleX = length(float3(sprite.world[0][0], sprite.world[0][1], sprite.world[0][2]));
        float scaleY = length(float3(sprite.world[1][0], sprite.world[1][1], sprite.world[1][2]));
        
        worldPos = float4(center + (right * localPos.x * scaleX) + (up * localPos.y * scaleY), 1.0f);
    } else {
        // 通常の配置
        worldPos = mul(float4(localPos, 0.0f, 1.0f), sprite.world);
    }
    
    output.position = mul(worldPos, gSceneData.viewProj);
    output.uv = uv;
    output.instanceID = iID;
    
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    SpriteData sprite = gSprites[input.instanceID];
    float4 texColor = gTextures[NonUniformResourceIndex(sprite.textureIndex)].Sample(gSampler, input.uv);
    return texColor * sprite.color;
}
