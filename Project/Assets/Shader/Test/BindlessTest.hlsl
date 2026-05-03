struct Vertex {
    float3 position;
    float2 uv;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

// --- Bindless Resources ---
// t0, space1 ~ : Textures
// s0, space0   : Sampler (Static)
Texture2D gTextures[] : register(t0, space1); 
SamplerState gSampler : register(s0);

// --- Proper Mesh Resources ---
// t0, space0 : Vertex Buffer (Manual Fetching)
StructuredBuffer<Vertex> gVertices : register(t0, space0); 

// --- Per Frame/Scene Data ---
struct SceneData {
    float4x4 viewProj;
};
ConstantBuffer<SceneData> gSceneData : register(b0);

// --- Per Object/Material Data ---
struct MaterialData {
    uint textureIndex;
    // other params...
};
ConstantBuffer<MaterialData> gMaterial : register(b1);


VSOutput vs_main(uint vID : SV_VertexID) {
    VSOutput output;
    
    // 頂点バッファからデータを取得 (Manual Vertex Fetching)
    Vertex v = gVertices[vID];
    
    output.position = mul(float4(v.position, 1.0f), gSceneData.viewProj);
    output.uv = v.uv;
    
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    uint texIdx = gMaterial.textureIndex;

    // テクスチャをサンプリング
    float4 color = gTextures[NonUniformResourceIndex(texIdx)].Sample(gSampler, input.uv);

    // 最終的な色を出力
    return float4(color.rgb, 1.0f);
}
