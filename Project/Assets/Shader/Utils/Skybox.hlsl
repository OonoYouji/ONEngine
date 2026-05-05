struct VSOutput {
    float4 position : SV_POSITION;
    float3 viewDir : TEXCOORD;
};

#include "../Schema/Schema.hlsli"

ConstantBuffer<SceneData> gSceneData : register(b0);

// --- Bindless ---
TextureCube gSkybox : register(t0, space1);
SamplerState gSampler : register(s0);

VSOutput vs_main(uint vID : SV_VertexID) {
    VSOutput output;
    
    // 全画面三角形から視線ベクトルを生成
    float2 uv = float2((vID << 1) & 2, vID & 2);
    float4 pos = float4(uv * float2(2, -2) + float2(-1, 1), 0.999999f, 1.0f); // 深度を最遠に
    
    // 逆ビュー・プロジェクション行列でワールド空間の視線ベクトルを復元
    // TODO: 効率化のため SceneData に逆行列を渡すか、あるいは頂点シェーダー内で簡易計算
    // 簡易版: viewProj の逆行列を使用 (SceneData に含める必要がある)
    
    // 現在の設計では viewProj しかないため、ピクセルシェーダー側で補完された座標から計算するか、
    // あるいは頂点シェーダーに直接逆行列を渡すように設計変更する。
    
    output.position = pos;
    output.viewDir = float3(0, 0, 0); // TODO
    
    return output;
}

// 別のアプローチ: 巨大な立方体を描画する
VSOutput vs_cube(uint vID : SV_VertexID) {
    VSOutput output;
    
    // 単位立方体の頂点データ (手動生成)
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
    
    // カメラ位置を反映（平行移動のみ排除したビュー行列を掛ける）
    // 簡易的に cameraPos を足す
    float4 worldPos = float4(pos * 500.0f + gSceneData.cameraPos, 1.0f);
    output.position = mul(worldPos, gSceneData.viewProj);
    output.position.z = output.position.w; // 深度を最遠 (1.0) に固定
    output.viewDir = pos;
    
    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET {
    // スカイボックスのテクスチャをサンプリング
    float3 dir = normalize(input.viewDir);
    float4 color = gSkybox.Sample(gSampler, dir);
    return color;
}
