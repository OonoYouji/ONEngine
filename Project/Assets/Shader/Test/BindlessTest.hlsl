struct Vertex {
    float3 position;
    float2 uv;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

// --- Bindless Resources ---
// t0 ~ : Textures
// s0 ~ : Samplers
Texture2D gTextures[] : register(t0); 
SamplerState gSampler : register(s0);

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
    
    // 今回は頂点データをシェーダー内で完結させる
    float3 positions[3] = {
        { -0.5f, -0.5f, 0.0f },
        {  0.0f,  0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f }
    };
    float2 uvs[3] = {
        { 0.0f, 1.0f },
        { 0.5f, 0.0f },
        { 1.0f, 1.0f }
    };
    
    output.position = mul(float4(positions[vID % 3], 1.0f), gSceneData.viewProj);
    output.uv = uvs[vID % 3];
    
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    uint texIdx = gMaterial.textureIndex;

    // テクスチャをサンプリング
    float4 color = gTextures[NonUniformResourceIndex(texIdx)].Sample(gSampler, input.uv);

    // 最終的な色を出力（アルファはテクスチャに従うか、1.0固定）
    return float4(color.rgb, 1.0f);
}
