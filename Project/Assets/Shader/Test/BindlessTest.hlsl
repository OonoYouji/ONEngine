struct Vertex {
    float3 position;
    float2 uv;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    uint instanceID : SV_InstanceID;
};

// --- Bindless Resources ---
// t0, space1 ~ : Textures
// s0, space0   : Sampler (Static)
Texture2D gTextures[] : register(t0, space1); 
SamplerState gSampler : register(s0);

// --- Proper Mesh Resources ---
// t0, space0 : Vertex Buffer (Manual Fetching)
StructuredBuffer<Vertex> gVertices : register(t0, space0); 

#include "../Schema/Schema.hlsli"

ConstantBuffer<SceneData> gSceneData : register(b0);
StructuredBuffer<InstanceData> gInstances : register(t1, space0);

VSOutput vs_main(uint vID : SV_VertexID, uint iID : SV_InstanceID) {
    VSOutput output;
    
    // 頂点バッファからデータを取得
    Vertex v = gVertices[vID];
    
    // インスタンスデータを取得
    InstanceData inst = gInstances[iID];
    
    // World変換 -> ViewProjection変換
    float4 worldPos = mul(float4(v.position, 1.0f), inst.world);
    output.position = mul(worldPos, gSceneData.viewProj);
    output.uv = v.uv;
    output.instanceID = iID;
    
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    // インスタンスデータを取得
    InstanceData inst = gInstances[input.instanceID];
    uint texIdx = inst.textureIndex;

    // テクスチャをサンプリング
    float4 color = gTextures[NonUniformResourceIndex(texIdx)].Sample(gSampler, input.uv);

    // 最終的な色を出力
    return float4(color.rgb, 1.0f);
}
