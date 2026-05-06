struct VSOutput {
    float4 position : SV_POSITION;
    float3 viewDir : TEXCOORD;
};

#include "../Schema/Schema.hlsli"

ConstantBuffer<SceneData> gSceneData : register(b0);

// --- 2Dテクスチャとして受け取る (不一致を解消) ---
Texture2D gSkybox : register(t0, space1);
SamplerState gSampler : register(s0);

// 立方体を描画して視線ベクトルを生成
VSOutput vs_cube(uint vID : SV_VertexID) {
    VSOutput output;
    
    float3 vertices[8] = {
        float3(-1, -1, -1), float3(1, -1, -1), float3(1, 1, -1), float3(-1, 1, -1),
        float3(-1, -1, 1), float3(1, -1, 1), float3(1, 1, 1), float3(-1, 1, 1)
    };
    uint indices[36] = {
        0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7,
        0, 1, 5, 0, 5, 4, 2, 3, 7, 2, 7, 6,
        0, 4, 7, 0, 7, 3, 1, 2, 6, 1, 6, 5
    };

    float3 pos = vertices[indices[vID]];
    
    // カメラ位置に追従させる
    float4 worldPos = float4(pos * 500.0f + gSceneData.cameraPos, 1.0f);
    output.position = mul(worldPos, gSceneData.viewProj);
    output.position.z = output.position.w; // 最遠に固定
    output.viewDir = pos;
    
    return output;
}

struct PSOutput {
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    uint2 idFlags : SV_Target2;
};

PSOutput ps_main(VSOutput input) {
    float3 dir = normalize(input.viewDir);
    
    // 視線ベクトルからパノラマUV（Lat-Long）を計算
    const float2 invAtan = float2(0.1591, 0.3183);
    float2 uv = float2(atan2(dir.z, dir.x), asin(dir.y));
    uv *= invAtan;
    uv += 0.5;

    PSOutput output;
    output.color = gSkybox.Sample(gSampler, uv);
    output.normal = float4(0.5f, 0.5f, 0.5f, 1.0f); // 無効な法線
    output.idFlags = uint2(0, 0); // 背景はID 0
    return output;
}
